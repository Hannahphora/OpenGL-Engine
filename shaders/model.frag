#version 460 core
in vec2 TexCoords;
in vec3 FragPos;
in mat3 TBN;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

uniform vec3 lightPos;
uniform vec3 viewPos;

// controls intensity of parallax effect
uniform float heightScale;

// parallax mapping
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    // get height from height map
    float height = texture(texture_height1, texCoords).r;
    // offset tex coords along view direction in tangent space
    vec2 p = viewDir.xy / viewDir.z * (height * heightScale);
    return texCoords - p;
}

void main()
{
    // compute view dir in tangent space
    vec3 viewDir = normalize(TBN * (viewPos - FragPos));
    
    // apply parallax mapping
    vec2 texCoords = ParallaxMapping(TexCoords, viewDir);
    
    // discard frags if tex coords are out of bounds
    if(texCoords.x > 1.0 || texCoords.x < 0.0 || texCoords.y > 1.0 || texCoords.y < 0.0) discard;
    
    // sample normals and transform from [0,1] to [-1,1]
    vec3 norm = texture(texture_normal1, texCoords).rgb;
    norm = normalize(norm * 2.0 - 1.0);
    
    // sample diffuse
    vec3 color = texture(texture_diffuse1, texCoords).rgb;
    
    // ambient lighting
    vec3 ambient = 0.1 * color;
    
    // diffuse lighting
    vec3 lightDir = normalize(TBN * (lightPos - FragPos));
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;
    
    // specular lighting
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = texture(texture_specular1, texCoords).rgb * spec;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}