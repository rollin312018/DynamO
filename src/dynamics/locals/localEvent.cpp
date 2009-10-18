/*  DYNAMO:- Event driven molecular dynamics simulator 
    http://www.marcusbannerman.co.uk/dynamo
    Copyright (C) 2009  Marcus N Campbell Bannerman <m.bannerman@gmail.com>

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 3 as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "localEvent.hpp"
#include "local.hpp"
#include "../../base/is_exception.hpp"
#include "../../extcode/xmlwriter.hpp"
#include "../../extcode/xmlParser.h"
#include "../../simulation/particle.hpp"
#include "../../base/is_simdata.hpp"
#include "../units/units.hpp"
#include "../interactions/intEvent.hpp"
#include <cmath>

CLocalEvent::CLocalEvent(const CParticle& part1, const Iflt &delt, 
			 EEventType nType, const CLocal& local):
  particle_(&part1), dt(delt), 
  CType(nType), localID(local.getID())
{}
  
const char * 
CLocalEvent::getCollEnumName(EEventType a)
{
  return CIntEvent::getCollEnumName(a);
}

xmlw::XmlStream& operator<<(xmlw::XmlStream &XML, 
			    const CLocalEvent &coll)
{
  XML << xmlw::tag("Collision")
      << xmlw::attr("p1ID") << coll.getParticle().getID()
      << xmlw::attr("dt")   << coll.dt
      << xmlw::endtag("Collision");
  
  return XML;
}

std::string 
CLocalEvent::stringData(const DYNAMO::SimData* Sim) const
{
  std::ostringstream tmpstring;
  tmpstring << "dt :" << dt / Sim->dynamics.units().unitTime()
	    << "\nType :" << getCollEnumName(CType)
	    << "\nP1 :" << particle_->getID();
    return tmpstring.str();
}

bool 
CLocalEvent::areInvolved(const CIntEvent &coll) const 
{ return (coll == *particle_); }
