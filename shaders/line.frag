#version 330 core

in float vSide;
in float vProj;
out vec4 col;

uniform vec3 uColor;
uniform float uBrightAmount;
uniform float uPixelSize;

void main() {
    float s = vSide;
    float p = vProj;
    float px = uPixelSize;

    if(px > 0.0) {
        vec2 snapPos = (floor(gl_FragCoord.xy / px) + 0.5) * px;
        vec2 d = snapPos - gl_FragCoord.xy;

        float dsx = dFdx(vSide);
        float dsy = dFdy(vSide);
        float dpx = dFdx(vProj);
        float dpy = dFdy(vProj);

        s = vSide + dsx * d.x + dsy * d.y;
        p = vProj + dpx * d.x + dpy * d.y;

        float inflate = 0.5;
        if(abs(s) > 1.0 + inflate * (1.0 / px)) discard;
    }

    float f = 1.0 + p * uBrightAmount;
    col = vec4(uColor * f, 1.0);
}
