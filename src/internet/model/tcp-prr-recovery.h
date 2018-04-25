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

#ifndef TcpPrrRecovery_H
#define TcpPrrRecovery_H

#include "ns3/tcp-recovery-ops.h"
#include "tcp-congestion-ops.h"

namespace ns3 {

/**
 * \ingroup recoveryOps
 *
 * \brief An implementation of PRR
 *
 * PRR is an algorithm that determines TCP's sending rate in fast
 * recovery. PRR avoids excessive window reductions and aims for
 * the actual congestion window size at the end of recovery to be as
 * close as possible to the window determined by the congestion control
 * algorithm. PRR also improves accuracy of the amount of data sent
 * during loss recovery.
 */
class PrrRecovery : public ClassicRecovery
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Create an unbound tcp socket.
   */
  PrrRecovery (void);

  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  PrrRecovery (const PrrRecovery& sock);
  virtual ~PrrRecovery (void);

  typedef enum
  {
    CRB,
    SSRB
  } ReductionBound_t;

  std::string GetName () const;

  virtual void EnterRecovery (Ptr<TcpSocketState> tcb, uint32_t dupAckCount,
                              bool isSackEnabled, uint32_t unAckDataCount);

  virtual void DoRecovery (Ptr<TcpSocketState> tcb, uint32_t lastAckedBytes,
                           uint32_t lastSackedBytes, bool isDupack);

  virtual void ExitRecovery (Ptr<TcpSocketState> tcb);

  virtual void UpdateBytesSent (uint32_t bytesSent);

  virtual Ptr<TcpRecoveryOps> Fork ();

private:
  uint32_t m_prrDelivered                   {0};
  uint32_t m_prrOut                         {0};
  uint32_t m_recoveryFlightSize             {0};
  bool m_isSackEnabled                      {true};
  uint32_t m_dupAckCount                    {0};
  uint32_t m_previousSackedBytes            {0};
  ReductionBound_t m_reductionBoundMode     {CRB};
};


} // namespace ns3

#endif // TcpPrrRecovery_H
