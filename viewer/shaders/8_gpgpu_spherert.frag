#version 410
#define M_PI 3.14159265358979323846

uniform mat4 mat_inverse;
uniform mat4 persp_inverse;
uniform sampler2D envMap;
uniform vec3 center;
uniform float radius;

uniform bool transparent;
uniform float shininess;
uniform float eta;

in vec4 position;

out vec4 fragColor;


vec4 getColorFromEnvironment(in vec3 direction)
{
    // TODO
    // return texture2D
    return vec4(1);
}

// Fresnel coefficient computation
float F( float cos_theta )
{
    float ci = sqrt(pow(eta, 2) - (1 - pow(cos_theta, 2)));
    float Fs = pow(abs((cos_theta - ci) / (cos_theta + ci)), 2);
    float Fp = pow(abs((pow(eta, 2) * cos_theta - ci) / (pow(eta, 2) * cos_theta + ci)), 2);

    return (Fs + Fp) / 2;
}

bool raySphereIntersect(in vec3 start, in vec3 direction, out vec3 newPoint) {
    vec3 u = normalize(direction);
    vec3 CP = normalize(start - center);
    float delta = (pow(dot(u, CP),2) - pow(dot(CP, CP), 2) + pow(radius, 2));
    if (delta > 0){
        float b = dot(u, CP);
        float lambda = - b - sqrt(delta);
        if (lambda <= 0) {
            lambda = -b + sqrt(delta);
        }
        newPoint = start + lambda * u;
        return true;
    }
    return false;
}

void main(void)
{
    // Step 1: I need pixel coordinates. Division by w?
    vec4 worldPos = position;
    worldPos.z = 1; // near clipping plane
    worldPos = persp_inverse * worldPos;
    worldPos /= worldPos.w;
    worldPos.w = 0;
    worldPos = normalize(worldPos);
    // Step 2: ray direction:
    vec3 u = normalize((mat_inverse * worldPos).xyz);
    vec3 eye = (mat_inverse * vec4(0, 0, 0, 1)).xyz;

    vec3 start = eye;
    vec3 newPoint = vec3(0);
    // Testing if the ray hits the sphere the 1st time
    if (raySphereIntersect(start, u, newPoint)) {
        vec3 normSphere = normalize(normalize(newPoint - center));
        vec3 u_reflected = reflect(u, normSphere);
        vec3 u_refracted = refract(u, normSphere, eta);
        float cos_theta = dot(normSphere, u_reflected);
        float F_theta = F(cos_theta);
        u_reflected = F_theta * u_reflected;
        u_refracted = (1 - F_theta) * u_refracted;
        // The current ray inside the sphere is then u
    }


    vec4 resultColor = vec4(0,0,0,1);
    fragColor = resultColor;
}
