//
//  Billboard.vsh
//
//  Created by José Miguel Santana Núñez
//

attribute vec4 Position;
attribute vec2 TextureCoord;
attribute vec4 Color;

uniform mat4 Projection;
uniform mat4 Modelview;

uniform vec2 TextureExtent;
uniform vec2 ViewPortExtent;

varying vec4 VertexColor;
varying vec2 TextureCoordOut;

void main() {
  gl_Position = Projection * Modelview * Position;
  
  gl_Position.x += ((TextureCoord.x - 0.5) * 2.0 * TextureExtent.x / ViewPortExtent.x) * gl_Position.w;
  gl_Position.y -= ((TextureCoord.y - 0.5) * 2.0 * TextureExtent.y / ViewPortExtent.y) * gl_Position.w;
  
  TextureCoordOut = TextureCoord;
  
  VertexColor = Color; //Needed ???
}
