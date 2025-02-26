#version 460 core

out vec4 FragColor;
in vec2 vTexCoords;

uniform sampler2D uScreenTexture;

uniform float uBlackHoleAABBx;
uniform float delta;
uniform vec2 uBlackHoleCenterPos;
uniform vec2 uCurrentResolution;

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

void main() {
	
	float alpha = 0;

	float rand = random(vec3(gl_FragCoord.x, gl_FragCoord.y, delta));

	float area = 300.0 / 1920.0 * uCurrentResolution.x;
	float area2 = 800.0 / 1920.0 * uCurrentResolution.x;
	float secondStepMod = 1.5;

	if (gl_FragCoord.x < uBlackHoleAABBx) {
		if ((uBlackHoleAABBx - gl_FragCoord.x) < area) {
			float noise = noise1(delta * 10000.0 + gl_FragCoord.x / 100.0 + gl_FragCoord.y / 100.0);

			float alpha;

			float threshold = (uBlackHoleAABBx - gl_FragCoord.x) / area;

			if (rand > (threshold + 0.03) * 0.95) {
				alpha = 1;
			}
			FragColor = texture(uScreenTexture, vTexCoords) * vec4(1, 1, 1 , alpha);
		} else if ((uBlackHoleAABBx - gl_FragCoord.x) > (area - 10.0 / 1920.0 * uCurrentResolution.x) && (uBlackHoleAABBx - gl_FragCoord.x) < area * 2.0) {

			float threshold = (((uBlackHoleAABBx - area)- gl_FragCoord.x) / area);

			if (rand > 0.96 + (threshold * 0.3) ) {
				alpha = 1;
			}

			FragColor = texture(uScreenTexture, vTexCoords) * vec4(1, 1, 1, alpha);
		} 
		if ((uBlackHoleAABBx - gl_FragCoord.x) > area + (40.0 / 1920.0 * uCurrentResolution.x)) {
			float rand = random(vec3(gl_FragCoord.x, gl_FragCoord.y, delta));
			if (rand > 0.999) {
				alpha = 1;
			} else {
				alpha = 0;
			}

			bool toDraw = true;

			if (uBlackHoleCenterPos.x + area2 > gl_FragCoord.x) {
				float offset = ((gl_FragCoord.x - uBlackHoleCenterPos.x)) / area2;
				if (gl_FragCoord.y > (uBlackHoleCenterPos.y + uBlackHoleCenterPos.y * offset)) {
					toDraw = false;
				} else if (gl_FragCoord.y < (uBlackHoleCenterPos.y - uBlackHoleCenterPos.y * offset)) {
					toDraw = false;
				}
			}

			if (toDraw) {
				FragColor = texture(uScreenTexture, vTexCoords) * vec4(1, 1, 1 , alpha);
			}
		}
	} else {
		
			FragColor = texture(uScreenTexture, vTexCoords);
	}



}