//
//  Default.vsh
//
//  Created by José Miguel Santana Núñez
//

attribute vec4 Position;
attribute vec2 TextureCoord;
attribute vec4 Color;

uniform mediump vec2 TranslationTexCoord;
uniform mediump vec2 ScaleTexCoord;

uniform mat4 Projection;
uniform mat4 Modelview;

uniform float PointSize;

varying vec4 VertexColor;
varying vec2 TextureCoordOut;


void main() {
  gl_Position = Projection * Modelview * Position;
  
  TextureCoordOut = (TextureCoord * ScaleTexCoord) + TranslationTexCoord;
  
  VertexColor = Color;
  
  gl_PointSize = PointSize;
}
