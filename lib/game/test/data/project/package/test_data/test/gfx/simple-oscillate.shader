
type = glsl
prelude = []

fixed_param_blocks = [
	{name = "ColorFactors" index = 0}
]

draw_param_blocks = [
	{name = "Oscillator"}
]

shared_source = ```
	f32 sinu(f32 x) {
		return abs(sin(x));
	}

	PARAM_BLOCK_FIXED(ColorFactors, 0) {
		Vec2 rg_gb;
	} p_color_factors;

	PARAM_BLOCK_DRAW(Oscillator) {
		f32 time;
	} p_osc;

	FRAG_ATTR Vec3 frag_color;
```

vertex_source = ```
	VERT_ATTR(0) Vec2 a_pos;
	VERT_ATTR(1) Vec3 a_color;

	MAIN {
		// frag_color = a_color;
		frag_color = Vec3(
			a_color.r * p_color_factors.rg_gb.x,
			a_color.g * (0.5 * p_color_factors.rg_gb.x + 0.5 * p_color_factors.rg_gb.y),
			a_color.b * p_color_factors.rg_gb.y
		);
		RESULT = Vec4(a_pos * sinu(0.60 * p_osc.time), 0.0, 1.0);
		// RESULT = Vec4(a_pos, 0.0, 1.0);
	}
```

fragment_source = ```
	FRAG_RESULT(0) Vec4 RESULT0;

	MAIN {
		Vec3 c = 1.0 - frag_color;
		RESULT0 = Vec4(c * sinu(0.60 * p_osc.time), 1.0);
		// RESULT0 = Vec4(c, 1.0);
	}
```
