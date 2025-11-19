
#version 330 core
out vec4 FragColor;
in vec3 vColor;
in vec2 vTex;  
in vec3 vNormal;  
in vec3 vFragPos; 

uniform sampler2D tex0;
uniform vec3 lightPos;     // posizione cubo-luce
uniform vec3 lightColor;   // es. (1,1,1)
uniform vec3 camPos;       // posizione camera

void main() {

    vec3 albedo = texture(tex0, vTex).rgb; 

    float ambient = 0.20f;
    vec3 N = normalize(vNormal);
    vec3 L = normalize(lightPos - vFragPos);    //direzione della luce
    float diff = max(dot(N, L), 0.0);

    vec3 V = normalize(camPos - vFragPos);
    vec3 R = reflect(-L, N);
    float specAmount = pow(max(dot(V, R), 0.0), 32.0);
    float ks = 0.5;

    vec3 lighting = lightColor * (ambient + diff + ks * specAmount);
    vec3 color = albedo * lighting;

    FragColor = vec4(color, 1.0);
    
   
}
   //Colore figura, texture modulata dal colore interpolato