/*****************************************************************************/
/*                                                                           */
/*  Visualization Library                                                    */
/*  http://www.visualizationlibrary.com                                      */
/*                                                                           */
/*  Copyright 2005-2009 Michele Bosi, all rights are reserved                */
/*                                                                           */
/*  This file is part of Visualization Library.                              */
/*  It may only be used, modified, and distributed under the terms of the    */
/*  license specified in the LICENSE.TXT file. By continuing to use,         */
/*  modify or distribute this file you indicate that you have read the       */
/*  license and understand and accept it fully.                              */
/*                                                                           */
/*  Visualization Library is distributed in the hope that it will be useful, */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the LICENSE.TXT */
/*  file for more details.                                                   */
/*                                                                           */
/*****************************************************************************/

#ifndef Sphere_INCLUDE_ONCE
#define Sphere_INCLUDE_ONCE

#include <vl/AABB.hpp>

namespace vl
{
//-----------------------------------------------------------------------------
// Sphere
//-----------------------------------------------------------------------------
  /**
   * The Sphere class defines a sphere using a center and a radius using vl::Real precision.
  */
  class Sphere
  {
  public:
    Sphere()
    {
      mRadius = -1;
    }
    Sphere(const AABB& aabb)
    {
      *this = aabb;
    }
    void setNull()  { mRadius =-1.0f; mCenter = vec3(0,0,0); }
    void setPoint() { mRadius = 0.0f; /*mCenter = vec3(0,0,0);*/ }
    bool isNull()  const { return mRadius <  0.0f; }
    bool isPoint() const { return mRadius == 0.0f; }
    void setCenter(const vec3& center) { mCenter = center; }
    const vec3& center() const { return mCenter; }
    void setRadius( Real radius ) { mRadius = radius; }
    Real radius() const { return mRadius; }

    bool includes(const Sphere& other) const
    {
      if (isNull())
        return false;
      else
      if (other.isNull())
        return true;
      else
      {
        Real distance = (center() - other.center()).length();
        return radius() >= distance + other.radius();
      }
    }
    
    Sphere& operator=(const AABB& aabb)
    {
      // center the sphere on the aabb center
      mCenter = aabb.center();
      // half of the maximum diagonal
      mRadius = (aabb.minCorner() - aabb.maxCorner()).length() / (Real)2.0;
      return *this;
    }

    Sphere operator+(const Sphere& other)
    {
      Sphere t = *this;
      return t += other;
    }

    const Sphere& operator+=(const Sphere& other)
    {
      if (this->isNull())
        *this = other;
      else
      if (other.includes(*this))
      {
        *this = other;
      }
      else
      if (!other.isNull() && !this->includes(other) )
      {
        vec3 v = other.center() - this->center();
        if (v.isNull())
        {
          // the center remains the same
          // sets the maximum radius
          setRadius( radius() > other.radius() ? radius() : other.radius() );
        }
        else
        {
          v.normalize();
          vec3 p0 = this->center() - v * this->radius();
          vec3 p1 = other.center() + v * other.radius();
          setCenter( (p0 + p1)*(Real)0.5 );
          setRadius( (p0 - p1).length()*(Real)0.5 );
        }
      }

      return *this;
    }

    void transformed(Sphere& sphere, const mat4& mat) const 
    {
      sphere.setNull();
      if ( !isNull() )
      {
        sphere.mCenter = mat * center();
        // vec3 p = center() + vec3( (Real)0.577350269189625840, (Real)0.577350269189625840, (Real)0.577350269189625840 ) * radius();
        // p = mat * p;
        // p = p - sphere.center();
        // sphere.setRadius(p.length());
        vec3 p0 = center() + vec3(radius(),0,0);
        vec3 p1 = center() + vec3(0,radius(),0);
        vec3 p2 = center() + vec3(0,0,radius());
        p0 = mat * p0;
        p1 = mat * p1;
        p2 = mat * p2;
        Real d0 = (p0 - sphere.mCenter).lengthSquared();
        Real d1 = (p1 - sphere.mCenter).lengthSquared();
        Real d2 = (p2 - sphere.mCenter).lengthSquared();
        sphere.mRadius = d0>d1 ? (d0>d2?d0:d2) : (d1>d2?d1:d2);
        sphere.mRadius = ::sqrt(sphere.mRadius);
      }
    }

    Sphere transformed(const mat4& mat) const 
    {
      Sphere sphere;
      transformed(sphere, mat);
      return sphere;
    }

  protected:
    vec3 mCenter;
    Real mRadius;
  };
}

#endif
