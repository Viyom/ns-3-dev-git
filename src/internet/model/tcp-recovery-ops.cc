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
#include "tcp-recovery-ops.h"
#include "tcp-socket-base.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TcpRecoveryOps");

NS_OBJECT_ENSURE_REGISTERED (TcpRecoveryOps);

TypeId
TcpRecoveryOps::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpRecoveryOps")
    .SetParent<Object> ()
    .SetGroupName ("Internet")
  ;
  return tid;
}

TcpRecoveryOps::TcpRecoveryOps () : Object ()
{
}

TcpRecoveryOps::TcpRecoveryOps (const TcpRecoveryOps &other) : Object (other)
{
}

TcpRecoveryOps::~TcpRecoveryOps ()
{
}


// Classic recovery

NS_OBJECT_ENSURE_REGISTERED (ClassicRecovery);

TypeId
ClassicRecovery::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ClassicRecovery")
    .SetParent<TcpRecoveryOps> ()
    .SetGroupName ("Internet")
    .AddConstructor<ClassicRecovery> ()
  ;
  return tid;
}

ClassicRecovery::ClassicRecovery (void) : TcpRecoveryOps ()
{
  NS_LOG_FUNCTION (this);
}

ClassicRecovery::ClassicRecovery (const ClassicRecovery& sock)
  : TcpRecoveryOps (sock)
{
  NS_LOG_FUNCTION (this);
}

ClassicRecovery::~ClassicRecovery (void)
{
}

void
ClassicRecovery::EnterRecovery (Ptr<TcpSocketState> tcb)
{
  tcb->m_cWnd = tcb->m_ssThresh;
}

void
ClassicRecovery::DoRecovery ()
{
}

void
ClassicRecovery::ExitRecovery ()
{
}

std::string
ClassicRecovery::GetName () const
{
  return "ClassicRecovery";
}

Ptr<TcpRecoveryOps>
ClassicRecovery::Fork ()
{
  return CopyObject<ClassicRecovery> (this);
}

} // namespace ns3

