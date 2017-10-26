/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 University of 
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
 * Author: Takahiro Ono
 */

/**
 * \file generator-utils.cpp
 * \brief Utils of Generator object.
 * \author Takahiro Ono
 * \date 2017
 */

#include "generator-utils.h"

GenUtil::GenUtil(Generator *gen) : gen(gen) {}
GenUtil::~GenUtil() {}

bool GenUtil::searchNetworkHardwareNumber(size_t& result, const std::string& name) const {
  for(size_t i = 0; i < gen->GetNNetworkHardwares(); i++)
  {
    if(name == gen->GetNetworkHardware(i)->GetNetworkHardwareName())
    {
      result = i;
      return true;
    }
  }
  return false;
}

bool GenUtil::searchNodeNumber(size_t& result, const std::string& name) const {
  for(size_t i = 0; i < gen->GetNNodes(); i++)
  {
    if(name == gen->GetNode(i)->GetNodeName())
    {
      result = i;
      return true;
    }
  }
  return false;
}

void GenUtil::dumpNetworkHardwareNumber() const {
  for(size_t i = 0; i < gen->GetNNetworkHardwares(); i++)
  {
    std::cout << gen->GetNetworkHardware(i)->GetNetworkHardwareName() << std::endl;
  }
}

void GenUtil::dumpNodeNumber() const {
  for(size_t i = 0; i < gen->GetNNodes(); i++)
  {
    std::cout << gen->GetNode(i)->GetNodeName() << std::endl;
  }
}
