#!/bin/env python3

import sys

if len(sys.argv) != 2:
	sys.exit("need exactly one parameter: number of segments")

segcnt = int(sys.argv[1])

SHADER_SRC = """#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = %(maxvert)d) out;

uniform mat4 MVP;
uniform int segcount;

void main() {
	float len1 = gl_in[1].gl_Position.x;
	float len0 = gl_in[0].gl_Position.x;
	gl_Position = MVP * gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = MVP * gl_in[1].gl_Position;
	EmitVertex();
	for (int i=1; i<segcount; i++) {
		float angle = i * 2 * 3.14159 / %(maxsegments)d;
		// inner new vertex
		gl_Position = MVP * vec4(cos(angle)*len0,gl_in[0].gl_Position.y,sin(angle)*len0,1);
		EmitVertex();
		// outer new vertex
		gl_Position = MVP * vec4(cos(angle)*len1,gl_in[1].gl_Position.y,sin(angle)*len1,1);
		EmitVertex();
	}
	if (segcount == %(maxsegments)d) {
		gl_Position = MVP * gl_in[0].gl_Position;
		EmitVertex();
		gl_Position = MVP * gl_in[1].gl_Position;
		EmitVertex();
	}
    EndPrimitive();
}"""

values = {
	"maxvert" : segcnt*2+2,
	"maxsegments" : segcnt,
}

print(SHADER_SRC % values)


