#version 410
#define M_PI 3.14159265358979323846
#define eps 0.001

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
    // Computation of spherical coordinates
    vec3 u_direction = normalize(direction);
    float phi = acos(u_direction.y);
    float theta = atan(u_direction.z, u_direction.x);
    return texture2D(envMap, vec2((theta+M_PI) / (2*M_PI), phi / M_PI));
}
//
// // Fresnel coefficient computation
// float F( float cos_theta, float eta)
// {
//     float ci = sqrt(pow(eta, 2) - (1 - pow(cos_theta, 2)));
//     float Fs = pow(abs((cos_theta - ci) / (cos_theta + ci)), 2);
//     float Fp = pow(abs((pow(eta, 2) * cos_theta - ci) / (pow(eta, 2) * cos_theta + ci)), 2);
//
//     return (Fs + Fp) / 2;
// }

float F( float cos_theta, float eta )
{
    float inv_eta = eta;
    if ((pow(inv_eta, 2) - (1 - pow(cos_theta, 2))) < 0) {
        return 1.0;
    }
    float ci = sqrt(pow(inv_eta, 2) - (1 - pow(cos_theta, 2)));
    float Fs = pow(abs((cos_theta - ci) / (cos_theta + ci)), 2);
    float Fp = pow(abs((pow(inv_eta, 2) * cos_theta - ci) / (pow(inv_eta, 2) * cos_theta + ci)), 2);

    return (Fs + Fp) / 2;
}

bool raySphereIntersect(in vec3 start, in vec3 direction, out vec3 newPoint) {
    vec3 u = normalize(direction);
    vec3 CP = start - center;
    float delta = (pow(dot(u, CP),2) - dot(CP, CP) + pow(radius, 2));
    if (delta > 0){
        float b = dot(u, CP);
        float lambda = - b - sqrt(delta);
        if (lambda <= eps) {
            lambda = -b + sqrt(delta);
            if (lambda <= eps) {
                newPoint = start;
                return false;
            }
        }
        newPoint = start + lambda * u;
        return true;
    }
    newPoint = start;
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
    vec3 direction = normalize((mat_inverse * worldPos).xyz);
    vec3 eye = (mat_inverse * vec4(0, 0, 0, 1)).xyz;

    vec3 start = vec3(eye.xyz);
    vec3 newPoint = vec3(0);
    vec4 resultColor = vec4(0,0,0,1);
    vec3 normSphere = vec3(0);
    float current_Fresnel = 1.0;
    float current_eta = eta;
    vec3 reflected_direction = vec3(0);
    vec3 refracted_direction = vec3(0);
    int nb_rays = 0;
    int nb_wished_rebounbs = 14;
    if (!transparent) {
        nb_wished_rebounbs = 0;
    }
    bool bubble = false;
    // considering a single glass sphere
    while (!bubble && nb_rays < nb_wished_rebounbs+1) {
        if (raySphereIntersect(start, direction, newPoint)) {
            // swichting cases between outside the sphere and inside it
            if (nb_rays==0) {
                normSphere = normalize(newPoint - center);
            } else {
                normSphere = normalize(center - newPoint);
                current_eta = 1/eta;
            }
            reflected_direction = reflect(direction, normSphere);
            refracted_direction = refract(direction, normSphere, 1/current_eta);
            float cos_theta = dot(normSphere, normalize(reflected_direction));
            float fresnel = F(cos_theta, current_eta);
            if (nb_rays==0) {
                direction = vec3(refracted_direction.xyz);
                if (!transparent) {
                    fresnel = 1.0;
                }
                resultColor += fresnel * getColorFromEnvironment(reflected_direction);
                current_Fresnel *= (1 - fresnel);
            } else {
                direction = vec3(reflected_direction.xyz);
                resultColor += current_Fresnel * (1 - fresnel) * getColorFromEnvironment(refracted_direction);
                current_Fresnel *= fresnel;
            }
            start = vec3(newPoint.xyz);
            nb_rays++;
        } else {
            resultColor += getColorFromEnvironment(direction);
            break;
        }
    }


    fragColor = resultColor;
}
