#shader vertex
#version 330 core
layout(location=0)in vec2 vPosition;

//uniform vec2 offset;

void main()
{
    gl_Position=vec4(vPosition.x,vPosition.y,0.,1.);
}

#shader geometry
#version 330 core
layout(lines)in;
layout(triangle_strip,max_vertices=12)out;

out vec2 fVertexPos;// interpolated model-space position of vertices of triangle where circle is draw.
flat out vec2 Cpos;
out vec2 centerLine;
flat out int isEndRect;
uniform float Width;
uniform mat4 MVP;

vec4 rotate90CW(vec4 v)
{
    return vec4(v.y,v.x*-1.f,v.z,v.w);
}

void emitVertex(vec4 pos)
{
    fVertexPos=pos.xy;
    gl_Position=MVP*pos;
    EmitVertex();
}

void main()
{
    isEndRect=0;
    centerLine=gl_in[1].gl_Position.xy-gl_in[0].gl_Position.xy;
    vec4 dir=normalize(gl_in[1].gl_Position-gl_in[0].gl_Position);
    vec4 normal=rotate90CW(dir);
    Cpos=gl_in[0].gl_Position.xy;
    
    gl_Position=(gl_in[0].gl_Position+Width*normal);
    emitVertex(gl_Position);
    gl_Position=(gl_in[0].gl_Position-Width*normal);
    emitVertex(gl_Position);
    gl_Position=(gl_in[1].gl_Position+Width*normal);
    emitVertex(gl_Position);
    gl_Position=(gl_in[1].gl_Position-Width*normal);
    emitVertex(gl_Position);
    
    EndPrimitive();
    
    isEndRect=1;
    gl_Position=(gl_in[0].gl_Position+Width*(normal));
    emitVertex(gl_Position);
    gl_Position=(gl_in[0].gl_Position-Width*(normal));
    emitVertex(gl_Position);
    gl_Position=(gl_in[0].gl_Position+Width*(normal-dir));
    emitVertex(gl_Position);
    gl_Position=(gl_in[0].gl_Position-Width*(normal+dir));
    emitVertex(gl_Position);
    EndPrimitive();
    
    Cpos=gl_in[1].gl_Position.xy;
    gl_Position=(gl_in[1].gl_Position+Width*(normal));
    emitVertex(gl_Position);
    gl_Position=(gl_in[1].gl_Position-Width*(normal));
    emitVertex(gl_Position);
    gl_Position=(gl_in[1].gl_Position+Width*(normal+dir));
    emitVertex(gl_Position);
    gl_Position=(gl_in[1].gl_Position-Width*(normal-dir));
    emitVertex(gl_Position);
    EndPrimitive();
}

#shader fragment
#version 330 core
layout(location=0)out vec4 vFragColor;

in vec2 fVertexPos;// interpolated model-space position of vertices of triangle where circle is draw.
flat in vec2 Cpos;
in vec2 centerLine;
flat in int isEndRect;
uniform float Width;

void main()
{
    float a=1;
    if(isEndRect==1)
    {
        float d=distance(Cpos,fVertexPos);
        a*=smoothstep(Width,Width-2,d);
    }
    else
    {
        float Clength=length(centerLine);
        vec2 Cnorm=normalize(centerLine);
        vec2 perpCnormed=vec2(Cnorm.y,-Cnorm.x);
        vec2 p13=fVertexPos-Cpos;
        float distanceFromLine=abs(dot(perpCnormed,p13));
        a*=smoothstep(Width,Width-2,distanceFromLine);
    }
    vFragColor=vec4(1.,1.,0,a);
}