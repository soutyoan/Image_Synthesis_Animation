#version 410

uniform float lightIntensity;
uniform bool blinnPhong;
uniform float shininess;
uniform float eta;
uniform sampler2D shadowMap;

in vec4 eyeVector;
in vec4 lightVector;
in vec4 vertColor;
in vec4 vertNormal;
in vec4 lightSpace;

out vec4 fragColor;

float compute_angle(vectA, vectB)
{
    return acos(dot(vectA, vectB) / (length(vectA) * length(vectB)));
}

float F( theta )
{
    float ci = sqrt(pow(eta, 2) - pow(sin(theta_d), 2));
    float Fs = pow(abs((cos(theta) - ci) / (cos(theta) + ci)), 2);
    float Fp = pow(abs((pow(eta, 2) * cos(theta) - ci) / (pow(eta, 2) * cos(theta) + ci)), 2);

    return (Fs + Fp) / 2;
}

float D( theta )
{
    // TODO 
}


void main( void )
{
     // This is the place where there's work to be done
     float ka = 1;
     float kd = 1;

     vec4 Ca = ka * lightIntensity * vertColor;
     vec4 Cd = kd * lightIntensity * vertColor * max(dot(vertNormal, lightVector), 0);

     vec4 halfVector = normalize(eyeVector + lightVector);
     float theta_d = compute_angle(halfVector, lightVector);

     vec4 Cs =  F(theta_d) * lightIntensity * pow(max(dot(vertNormal, halfVector), 0), shininess) * vertColor;

     fragColor = Ca + Cd + Cs;
}
