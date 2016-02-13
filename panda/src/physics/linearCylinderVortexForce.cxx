// Filename: linearCylinderVortexForce.cxx
// Created by:  charles (24Jul00)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) Carnegie Mellon University.  All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "config_physics.h"
#include "linearCylinderVortexForce.h"
#include "nearly_zero.h"
#include "cmath.h"

TypeHandle LinearCylinderVortexForce::_type_handle;

////////////////////////////////////////////////////////////////////
//     Function: LinearCylinderVortexForce
//       Access: Public
//  Description: Simple Constructor
////////////////////////////////////////////////////////////////////
LinearCylinderVortexForce::
LinearCylinderVortexForce(PN_stdfloat radius, PN_stdfloat length, PN_stdfloat coef,
                    PN_stdfloat a, bool md) :
  LinearForce(a, md),
  _radius(radius), _length(length), _coef(coef) {
}

////////////////////////////////////////////////////////////////////
//     Function: LinearCylinderVortexForce
//       Access: Public
//  Description: copy Constructor
////////////////////////////////////////////////////////////////////
LinearCylinderVortexForce::
LinearCylinderVortexForce(const LinearCylinderVortexForce &copy) :
  LinearForce(copy) {
  _radius = copy._radius;
  _length = copy._length;
  _coef = copy._coef;
}

////////////////////////////////////////////////////////////////////
//     Function: ~LinearCylinderVortexForce
//       Access: Public
//  Description: Destructor
////////////////////////////////////////////////////////////////////
LinearCylinderVortexForce::
~LinearCylinderVortexForce() {
}

////////////////////////////////////////////////////////////////////
//     Function: make_copy
//       Access: Public, Virtual
//  Description: child copier
////////////////////////////////////////////////////////////////////
LinearForce *LinearCylinderVortexForce::
make_copy() {
  return new LinearCylinderVortexForce(*this);
}

////////////////////////////////////////////////////////////////////
//     Function: get_child_vector
//       Access: Private, Virtual
//  Description: returns the centripetal force vector for the
//               passed-in object
////////////////////////////////////////////////////////////////////
LVector3 LinearCylinderVortexForce::
get_child_vector(const PhysicsObject *po) {
  // get the force-space transform- this MUST be the relative matrix
  // from the point's local coordinate system to the attached node's
  // local system.
  //  LMatrix4 force_space_xform = LMatrix4::ident_mat();
  LVector3 force_vec(0.0f, 0.0f, 0.0f);

  // project the point into force_space
  LPoint3 point = po->get_position();

  // clip along length
  if (point[2] < 0.0f || point[2] > _length)
    return force_vec;

  // clip to radius
  PN_stdfloat x_squared = point[0] * point[0];
  PN_stdfloat y_squared = point[1] * point[1];
  PN_stdfloat dist_squared = x_squared + y_squared;
  PN_stdfloat radius_squared = _radius * _radius;

  // squared space increases monotonically wrt linear space,
  // so there's no need to sqrt to check inside/outside this disc.
  if (dist_squared > radius_squared)
    return force_vec;

  if IS_NEARLY_ZERO(dist_squared)
    return force_vec;

  PN_stdfloat r = csqrt(dist_squared);

  if IS_NEARLY_ZERO(r)
    return force_vec;

  LVector3 tangential = point;
  tangential[2] = 0.0f;
  tangential.normalize();
  tangential = tangential.cross(LVector3(0,0,1));

  LVector3 centripetal = -point;
  centripetal[2] = 0.0f;
  centripetal.normalize();

  LVector3 combined = tangential + centripetal;
  combined.normalize();

  //  a = v^2 / r
  //centripetal = centripetal * _coef * (tangential.length_squared() /
  //                                     (r + get_nearly_zero_value(r)));

  centripetal = combined * _coef * po->get_velocity().length();

  //centripetal = combined * _coef * (po->get_velocity().length() /
  //                                  (r + get_nearly_zero_value(r)));

  return centripetal;
}

////////////////////////////////////////////////////////////////////
//     Function: output
//       Access: Public
//  Description: Write a string representation of this instance to
//               <out>.
////////////////////////////////////////////////////////////////////
void LinearCylinderVortexForce::
output(ostream &out) const {
  #ifndef NDEBUG //[
  out<<"LinearCylinderVortexForce";
  #endif //] NDEBUG
}

////////////////////////////////////////////////////////////////////
//     Function: write
//       Access: Public
//  Description: Write a string representation of this instance to
//               <out>.
////////////////////////////////////////////////////////////////////
void LinearCylinderVortexForce::
write(ostream &out, unsigned int indent) const {
  #ifndef NDEBUG //[
  out.width(indent); out<<""; out<<"LinearCylinderVortexForce:\n";
  LinearForce::write(out, indent+2);
  #endif //] NDEBUG
}
