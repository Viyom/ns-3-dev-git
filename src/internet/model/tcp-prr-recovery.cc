/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Viyom Mittal <viyommittal@gmail.com>
 *         Vivek Jain <jain.vivek.anand@gmail.com>
 *         Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 *
 */

#include "tcp-prr-recovery.h"
#include "ns3/tcp-socket-base.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PrrRecovery");
NS_OBJECT_ENSURE_REGISTERED (PrrRecovery);

TypeId
PrrRecovery::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PrrRecovery")
    .SetParent<ClassicRecovery> ()
    .AddConstructor<PrrRecovery> ()
    .SetGroupName ("Internet")
    .AddAttribute ("ReductionBound", "Type of Reduction Bound",
                   EnumValue (SSRB),
                   MakeEnumAccessor (&PrrRecovery::m_reductionBoundMode),
                   MakeEnumChecker (CRB, "CRB",
                                    SSRB, "SSRB"))
  ;
  return tid;
}

PrrRecovery::PrrRecovery (void)
  : ClassicRecovery ()
{
  NS_LOG_FUNCTION (this);
}

PrrRecovery::PrrRecovery (const PrrRecovery& recovery)
  : ClassicRecovery (recovery),
    m_prrDelivered (recovery.m_prrDelivered),
    m_prrOut (recovery.m_prrOut),
    m_recoveryFlightSize (recovery.m_recoveryFlightSize),
    m_isSackEnabled (recovery.m_isSackEnabled),
    m_dupAckCount (recovery.m_dupAckCount),
    m_previousSackedBytes (recovery.m_previousSackedBytes),
    m_reductionBoundMode (recovery.m_reductionBoundMode)
{
  NS_LOG_FUNCTION (this);
}

PrrRecovery::~PrrRecovery (void)
{
  NS_LOG_FUNCTION (this);
}

void
PrrRecovery::EnterRecovery (Ptr<TcpSocketState> tcb, uint32_t dupAckCount,
                            bool isSackEnabled, uint32_t unAckDataCount)
{
  NS_ASSERT_MSG (isSackEnabled == true, "Cannot perform recovery as SACK is disabled. " <<
                 "Enable SACK to perform recovery with prr");
  m_prrOut = 0;
  m_prrDelivered = 0;
  m_isSackEnabled = isSackEnabled;
  m_recoveryFlightSize = unAckDataCount;

  DoRecovery (tcb, 0, 0, false);
}

void
PrrRecovery::DoRecovery (Ptr<TcpSocketState> tcb, uint32_t lastAckedBytes,
                         uint32_t lastSackedBytes, bool isDupack)
{
  uint32_t lastDeliveredBytes;
  if (m_isSackEnabled)
    {
      int changeInSackedBytes = int (lastSackedBytes - m_previousSackedBytes);
      lastDeliveredBytes = lastAckedBytes + changeInSackedBytes > 0 ? lastAckedBytes + changeInSackedBytes : 0;
      m_previousSackedBytes = lastSackedBytes;
    }
  else
    {
      if (isDupack)
        {
          lastDeliveredBytes = tcb->m_segmentSize;
          m_dupAckCount++;
        }
      else
        {
          uint32_t bytesAcked = m_dupAckCount * tcb->m_segmentSize;
          lastDeliveredBytes = lastAckedBytes > bytesAcked ? lastAckedBytes - bytesAcked : 0;
          m_dupAckCount = 0;
        }
    }
  m_prrDelivered += lastDeliveredBytes;
  int sendCount;
  if (tcb->m_bytesInFlight > tcb->m_ssThresh)
    {
      sendCount = std::ceil (m_prrDelivered * tcb->m_ssThresh * 1.0 / m_recoveryFlightSize) - m_prrOut;
    }
  else
    {
      int limit;
      if (m_reductionBoundMode == CRB)
        {
          limit = m_prrDelivered - m_prrOut;
        }
      else if (m_reductionBoundMode == SSRB)
        {
          limit = std::max (m_prrDelivered - m_prrOut, lastDeliveredBytes) + tcb->m_segmentSize;
        }
      sendCount = std::min (limit, (int) (tcb->m_ssThresh - tcb->m_bytesInFlight));
    }

  /* Force a fast retransmit upon entering fast recovery */
  sendCount = std::max (sendCount, (m_prrOut ? 0 : (int)tcb->m_segmentSize));
  tcb->m_cWnd = tcb->m_bytesInFlight + sendCount;
  tcb->m_cWndInfl = tcb->m_cWnd;
}

void
PrrRecovery::ExitRecovery (Ptr<TcpSocketState> tcb)
{
  tcb->m_cWnd = tcb->m_ssThresh.Get ();
  tcb->m_cWndInfl = tcb->m_cWnd;
}

void
PrrRecovery::UpdateBytesSent (uint32_t bytesSent)
{
  m_prrOut += bytesSent;
}

Ptr<TcpRecoveryOps>
PrrRecovery::Fork (void)
{
  return CopyObject<PrrRecovery> (this);
}

std::string
PrrRecovery::GetName () const
{
  return "PrrRecovery";
}

} // namespace ns3
