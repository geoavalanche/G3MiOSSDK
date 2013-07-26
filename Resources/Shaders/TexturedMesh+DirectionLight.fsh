//
//  Default.vsh
//
//  Created by José Miguel Santana Núñez
//

precision highp float;

varying mediump vec2 TextureCoordOut;
varying mediump vec4 VertexColor;

uniform sampler2D Sampler;
uniform float uAmbientLight;

uniform vec4 uLightColor;

varying float diffuseLightIntensity;

void main() {
  gl_FragColor = texture2D(Sampler, TextureCoordOut);
  
//  vec4 lightColor = vec4(1.0,1.0,1.0,1.0) * uAmbientLight + uLightColor * diffuseLightIntensity;
//  gl_FragColor += lightColor;

  gl_FragColor *= (uAmbientLight + diffuseLightIntensity);
}