#version 410

uniform float lightIntensity;
uniform sampler2D colorTexture;
uniform bool blinnPhong;
uniform float shininess;
uniform float eta;
uniform sampler2D shadowMap;

in vec4 eyeVector;
in vec4 lightVector;
in vec4 vertColor;
in vec4 vertNormal;
in vec2 textCoords;
in vec4 lightSpace;

out vec4 fragColor;

#define EPS 0.0001
#define M_PI 3.1415926535897932384626433832795


float cos_angle(vec4 vectA, vec4 vectB)
{
    return dot(normalize(vectA), normalize(vectB));
}

float F( float cos_theta )
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


float G1(float alpha, float cos_theta)
{
    float tan_2_theta = (1 - pow(cos_theta, 2)) / pow(cos_theta, 2);
    return 2 / (1 + sqrt(1 + pow(alpha, 2) * tan_2_theta));
}

float D(float alpha, float cos_theta)
{
    if (cos_theta < 0) return 0.0;
    float tan_theta_2 = (1 - pow(cos_theta, 2)) / pow(cos_theta, 2);
    return pow(alpha, 2) / (M_PI * pow(cos_theta, 2) * pow((pow(alpha, 2) + tan_theta_2), 2));
}

void main( void )
{
    // This is the place where there's work to be done
    float ka = 0.2;
    float kd = 0.5;
    //vec2 textPlaneCoords = vec2(cos(textCoords.x), sin(textCoords.y));
    vec4 textColor = texture2D(colorTexture, textCoords);
    vec4 vertNormalN = normalize(vertNormal);
    vec4 lightVectorN = normalize(lightVector);
    vec4 eyeVectorN = normalize(eyeVector);

    vec4 Ca = ka * lightIntensity * textColor;

    // boolean value for shadow mapping : comparison bet. z_shadow and z_computed
    float z_shadow = texture2D(shadowMap, 0.5+0.5*lightSpace.xy).r;
    float z_computed = 0.5 + 0.5*lightSpace.z;



    // diffuse coefficient
    vec4 Cd = kd * lightIntensity * textColor * max(dot(vertNormalN, lightVectorN), 0);
    vec4 halfVector = normalize(eyeVectorN + lightVectorN);
    float cos_theta_d = cos_angle(halfVector, lightVectorN);

    // specular coefficient
    vec4 Cs =  F(cos_theta_d) * lightIntensity * pow(max(dot(vertNormalN, halfVector), 0), shininess) * textColor;

    // Cook-Torrance model computation
    if (!blinnPhong){
       float alpha = shininess / 200;
       float cos_theta_h = cos_angle(halfVector, vertNormalN);
       float cos_theta_i = cos_angle(lightVectorN, vertNormalN);
       float cos_theta_o = cos_angle(eyeVectorN, vertNormalN);
       Cs = (F(cos_theta_d) * D(alpha, cos_theta_h) * G1(alpha, cos_theta_i) * G1(alpha, cos_theta_o) / (4 * cos_theta_i * cos_theta_o)) * textColor;
    }

    if (z_shadow < z_computed - EPS) {
        // in shadow
        fragColor = Ca;
    } else {
        // should not happen in theory
        fragColor = Ca + Cd + Cs;
    }
}
