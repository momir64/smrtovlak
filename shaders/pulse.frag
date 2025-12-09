#version 330 core
in vec2 uv;
out vec4 col;

uniform float uStrength;
uniform float uOpacity;
uniform float uAspect;
uniform float uSpeed;
uniform float uSize;
uniform float uTime;
uniform vec3 uColor;
uniform float uPix;
uniform vec2 uPos;

float hash(float n) {
    return fract(sin(n * 91.3458) * 47453.1231);
}

void main() {
    vec2 suv = uv;
    suv.y /= uAspect;
    suv = floor(suv * uPix) / uPix;
    suv.y *= uAspect;

    vec2 p = suv - uPos;
    p.y /= uAspect;

    float d = length(p) / uSize;
    float t = uTime * uSpeed;

    float alpha = 0.0;
    for (int i = 0; i < 3; i++) {
        float wave = fract(t - float(i) / 3.0);
        float rnd = hash(float(i) * 17.123);
        float ring = abs(d - wave);
        float fadeIn = smoothstep(0.0, mix(0.1, 0.25, rnd), wave);
        float fadeOut = 1.0 - wave;
        float a = exp(-ring * uStrength * mix(20.0, 35.0, rnd)) * fadeIn * fadeOut * mix(0.7, 1.0, rnd);
        alpha += a;
    }

    col = vec4(uColor, clamp(alpha * uOpacity, 0.0, 1.0));
}
