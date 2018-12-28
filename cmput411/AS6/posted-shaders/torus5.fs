#version 330
/* sphere.fs
 * Taken from shadertoy.com with modification by Herb Yang
 */

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;

uniform float iTime;
in vec4 fragCoord;
out vec4 outColor;
/**
 * Signed distance function for a sphere centered at the origin with radius 1.0;
 */
float sphereSDF(vec3 samplePoint) {
    
    return length(samplePoint) - 1.0;
}
float torusSDF(vec3 samplePoint) {
    
    return sqrt(pow(sqrt (samplePoint.x*samplePoint.x + samplePoint.y*samplePoint.y)-0.3, 2) + samplePoint.z*samplePoint.z) - 0.05;
}
/**
 * Signed distance function describing the scene.
 * 
 * Absolute value of the return value indicates the distance to the surface.
 * Sign indicates whether the point is inside or outside the surface,
 * negative indicating inside.
 */
float sceneSDF(vec3 samplePoint) {
    return torusSDF(samplePoint);
}

/**
 * Return the shortest distance from the eyepoint to the scene surface along
 * the marching direction. If no part of the surface is found between start and end,
 * return end.
 * 
 * eye: the eye point, acting as the origin of the ray
 * marchingDirection: the normalized direction to march in
 * start: the starting distance away from the eye
 * end: the max distance away from the eye to march before giving up
 */
 float SDFF(vec3 eye, vec3 marchingDirection, float start, float end) {
    float depth = start;
    vec3 ori;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        ori = eye + depth * marchingDirection;
        ori.x = ori.x + 0.65;
        float dist = sceneSDF(ori);
        if (dist < EPSILON) {
			return depth;
        }
        depth += dist;
        if (depth >= end) {
            return end;
        }
    }
    return end;
}
float shortestDistanceToSurface(vec3 eye, vec3 marchingDirection, float start, float end) {
    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sceneSDF(eye + depth * marchingDirection);
        if (dist < EPSILON) {
			return depth;
        }
        depth += dist;
        if (depth >= end) {
            return end;
        }
    }
    return end;
}
            

/**
 * Return the normalized direction to march in from the eye point for a single pixel.
 * 
 * fieldOfView: vertical field of view in degrees
 * size: resolution of the output image
 * fragCoord: the x,y coordinate of the pixel in the output image
 */
vec3 rayDirection(float fieldOfView, vec2 fragCoord) {
    vec2 xy = fragCoord;
	float z = 1.0 / tan(radians(fieldOfView) / 2.0);// y goes from 1 to -1
    return normalize(vec3(xy, -z));
}

/**
 * Using the gradient of the SDF, estimate the normal on the surface at point p.
 */
vec3 estimateNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z  + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

/**
 * Lighting contribution of a single point light source via Phong illumination.
 * 
 * The vec3 returned is the RGB color of the light's contribution.
 *
 * k_a: Ambient color
 * k_d: Diffuse color
 * k_s: Specular color
 * alpha: Shininess coefficient
 * p: position of point being lit
 * eye: the position of the camera
 * lightPos: the position of the light
 * lightIntensity: color/intensity of the light
 *
 * See https://en.wikipedia.org/wiki/Phong_reflection_model#Description
 */
vec3 phongContribForLight(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 lightPos, vec3 lightIntensity) {
    vec3 N = estimateNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));
    
    float dotLN = dot(L, N);
    float dotRV = dot(R, V);
    
    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    } 
    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return lightIntensity * (k_d * dotLN);
    }
    return lightIntensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

/**
 * Lighting via Phong illumination.
 * 
 * The vec3 returned is the RGB color of that point after lighting is applied.
 * k_a: Ambient color
 * k_d: Diffuse color
 * k_s: Specular color
 * alpha: Shininess coefficient
 * p: position of point being lit
 * eye: the position of the camera
 *
 * See https://en.wikipedia.org/wiki/Phong_reflection_model#Description
 */
vec3 phongIllumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye) {
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;
    
    vec3 light1Pos = vec3(4.0 * sin(iTime),
                          2.0,
                          4.0 * cos(iTime));
    vec3 light1Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light1Pos,
                                  light1Intensity);
    
    vec3 light2Pos = vec3(2.0 * sin(0.37 * iTime),
                          2.0 * cos(0.37 * iTime),
                          2.0);
    vec3 light2Intensity = vec3(0.4, 0.4, 0.4);
    
    color += phongContribForLight(k_d, k_s, alpha, p, eye,
                                  light2Pos,
                                  light2Intensity);    
    return color;
}

float cmin(float arr[5],int num){
    float min = arr[0];

    for (int i = 0; i < num; i++)
    {
      
      if (arr[i] < min)
        {
          min = arr[i];
        }
    }
    return min;


}
void main(void )
{
	vec3 dir = rayDirection(45.0, fragCoord.xy);
    vec3 dir2 = rayDirection(45.0, fragCoord.xy);
    dir2.x = dir2.x +0.14;
    vec3 dir3 = dir;
    dir3.x = dir3.x -0.14;
    vec3 dir4 = dir;
    dir4.x = dir4.x + 0.068;
    dir4.y = dir4.y + 0.04;
    vec3 dir5 = dir4;
    dir5.x = dir5.x - 0.14;


    vec3 eye = vec3(0.0, 0.0, 5.0);
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    float dist2 = shortestDistanceToSurface(eye, dir2, MIN_DIST, MAX_DIST);
    float dist3 = shortestDistanceToSurface(eye, dir3, MIN_DIST, MAX_DIST);
    float dist4 = shortestDistanceToSurface(eye, dir4, MIN_DIST, MAX_DIST);
    float dist5 = shortestDistanceToSurface(eye, dir5, MIN_DIST, MAX_DIST);

    if (dist > MAX_DIST - EPSILON && dist2 > MAX_DIST - EPSILON && dist3 > MAX_DIST - EPSILON && dist4 > MAX_DIST - EPSILON && dist5 > MAX_DIST - EPSILON ) {
        // Didn't hit anything
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
    }
    
    // The closest point on the surface to the eyepoint along the view ray
    vec3 p = eye + dist * dir;
    vec3 p2 = eye + dist2 * dir2;
    vec3 p3 = eye + dist3 * dir3;
    vec3 p4 = eye + dist4 * dir4;
    vec3 p5 = eye + dist5 * dir5;

    float shininess = 1.0;

    vec3 K_a = vec3(0.1, 0.1, 0.1);
    vec3 K_d = vec3(0.1, 0.1, 0.1);
    vec3 K_s = vec3(1.0, 1.0, 1.0);

    vec3 K_a2 = vec3(0.1, 0.1, 0.9);
    vec3 K_d2 = vec3(0.1, 0.1, 0.9);
    vec3 K_s2 = vec3(1.0, 1.0, 1.0);

    vec3 K_a3 = vec3(0.9, 0.1, 0.1);
    vec3 K_d3 = vec3(0.9, 0.1, 0.1);
    vec3 K_s3 = vec3(1.0, 1.0, 1.0);

    vec3 K_a4 = vec3(0.8, 0.8, 0.1);
    vec3 K_d4 = vec3(0.8, 0.8, 0.1);
    vec3 K_s4 = vec3(1.0, 1.0, 1.0);

    vec3 K_a5 = vec3(0.1, 0.8, 0.1);
    vec3 K_d5 = vec3(0.1, 0.8, 0.1);
    vec3 K_s5 = vec3(1.0, 1.0, 1.0);

    // float shininess = 1.0;
    
    vec3 color1 = phongIllumination(K_a, K_d, K_s, shininess, p, eye);
    vec3 color2 = phongIllumination(K_a2, K_d2, K_s2, shininess, p2, eye);
    vec3 color3 = phongIllumination(K_a3, K_d3, K_s3, shininess, p3, eye);
    vec3 color4 = phongIllumination(K_a4, K_d4, K_s4, shininess, p4, eye);
    vec3 color5 = phongIllumination(K_a5, K_d5, K_s5, shininess, p5, eye);
    // if (dist <= MAX_DIST - EPSILON){
    //     vec4 (color1,1.0);
    // }
    // if (dist2 <= MAX_DIST - EPSILON){
    //     vec4 (color2,1.0);
    // }
    	
    float dists [5] = float[5](dist,dist2,dist3,dist4,dist5);
    if (cmin(dists,5) == dist) {
        outColor = vec4(color1, 1.0);
    }
    else if (cmin(dists,5) == dist2) {
        outColor = vec4(color2, 1.0);

    }
    else if (cmin(dists,5) == dist3){
        outColor = vec4(color3, 1.0);

    }
    else if (cmin(dists,5) == dist4) {
        outColor = vec4(color4, 1.0);

    }
    else{
        outColor = vec4(color5, 1.0);

    }
    }