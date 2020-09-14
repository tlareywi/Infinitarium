//
//  Copyright © 2018 Blue Canvas Studios LLC. All rights reserved.
//

struct VertexIn {
   vec4 position;
   float pointSize;
   vec4 color;
   float brightness;
   float diskDensity;
   float haloDensity;
   float haloBrightness;
   float diskBrightness;
   vec2 pointCenter;
   uint id;
};

layout(location = 0) in VertexIn point;

struct FragmentOut {
   vec4 color;
//   float pick;
};

layout(location = 0) out FragmentOut fragment;

vec4 main() {
   vec2 offset = point.position.xy - point.pointCenter;
   float len = dot(offset, offset);
   
   float disk = point.diskBrightness * point.brightness * exp(-len / (2.0 * point.diskDensity));
   float psf = point.haloBrightness * point.brightness * exp(-len / (2.0 * point.haloDensity));
   
   fragment.color = vec4( point.color.rgb * (disk+psf), 1.0 );
 //  if( len < 6.0f )
  //    fragment.pick = float(point.id);
  // else
  //    fragment.pick = 0.0f;

   return fragment;
}

