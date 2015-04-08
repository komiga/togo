
type = glsl
prelude = []

fixed_param_blocks = []

draw_param_blocks = []

shared_source = ```
	FRAG_ATTR Vec4 frag_color;
```

vertex_source = ```
	VERT_ATTR(0) Vec2 a_pos;
	VERT_ATTR(1) Vec3 a_color;

	MAIN {
		frag_color = Vec4(a_color, 1.0);
		RESULT = Vec4(a_pos, 0.0, 1.0);
	}
```

fragment_source = ```
	FRAG_RESULT(0) Vec4 RESULT0;

	MAIN {
		RESULT0 = frag_color;
	}
```
