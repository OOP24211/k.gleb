#version 120

uniform vec4  u_base_color;
uniform vec2  u_center;
uniform float u_radius;

void main()
{
    vec2 fragPos = gl_FragCoord.xy;
    vec2 toCenter = fragPos - u_center;
    float dist = length(toCenter);

    if (dist > u_radius) {
        discard;
    }

    // Нормаль сферы
    vec3 normal = normalize(vec3(toCenter / u_radius, sqrt(1.0 - dot(toCenter, toCenter) / (u_radius * u_radius))));

    vec3 lightDir = normalize(vec3(-1.0, 1.0, 1.0));

ъ    float diff = max(dot(normal, lightDir), 0.0);
    diff = diff * 0.8 + 0.3;

    // Блик
    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 10.0);
    vec3 specular = vec3(1.0) * spec * 1.2;


    vec3 color = u_base_color.rgb;
    vec3 litColor = color * diff + specular;

    // (свечение по краю)
    float rim = 1.0 - dot(viewDir, normal);
    rim = pow(rim, 3.0) * 0.4;
    litColor += color * rim;

    gl_FragColor = vec4(litColor, u_base_color.a);
}