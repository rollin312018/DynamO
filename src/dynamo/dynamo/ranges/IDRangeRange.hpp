/*  dynamo:- Event driven molecular dynamics simulator 
    http://www.dynamomd.org
    Copyright (C) 2011  Marcus N Campbell Bannerman <m.bannerman@gmail.com>

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

#pragma once
#include <dynamo/ranges/IDRange.hpp>
#include <dynamo/particle.hpp>
#include <magnet/exception.hpp>

namespace dynamo {
  class IDRangeRange: public IDRange
  {
  public:
    IDRangeRange(const magnet::xml::Node& XML) 
    { 
      try {
	startID = XML.getAttribute("Start").as<size_t>();
	endID = XML.getAttribute("End").as<size_t>() + 1;
      }
      catch (boost::bad_lexical_cast &)
	{ M_throw() << "Failed a lexical cast in IDRangeRange"; }
    }
    
    IDRangeRange(size_t s, size_t e):startID(s), endID(e) {}

    virtual bool isInRange(const Particle& part) const
    { return (part.getID() >= startID) && (part.getID() < endID); }

    virtual unsigned long size() const { return endID - startID; };

    virtual unsigned long operator[](unsigned long i) const  
    { return startID + i; }

    virtual unsigned long at(unsigned long i) const 
    { 
      if (i >= endID - startID)
	M_throw() << "Bad array access value in range.at()";
    
      return startID + i;
    }

  protected:
    virtual void outputXML(magnet::xml::XmlStream& XML) const
    {
      XML << magnet::xml::attr("Type") << "Ranged"
	  << magnet::xml::attr("Start") << startID
	  << magnet::xml::attr("End") << endID - 1;
    }

    size_t startID;
    size_t endID;
  };
}