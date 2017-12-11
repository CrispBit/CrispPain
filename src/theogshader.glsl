vec3 endp1 = vec3(-1.0, -1.0, 8.0);
vec3 endp2 = vec3(1.0, 1.0, 13.0);
float radius = 1.0;
float aspect; // = iResolution.x / iResolution.y;
vec3 campos = vec3(7.0, 0.0, -7.0);
vec3 camdir = vec3(-1.0, 0.0, 1.0);
float fov = 40.0;
const float PI = 3.141592653589793238;
vec3 da_sun = vec3(-1.0, -1.0, 0.6);
const vec4 skie = vec4(135.0/255.0, 206.0/255.0, 235.0/255.0, 1.0);
const vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0) * skie;

struct Ray {
    vec3 pos;
    vec3 dir;
};

vec4 lighting(in vec4 color, in vec3 normal) {
    float diffuse = max(dot(-da_sun, normal), 0.0);
    return color * (ambient + diffuse);
}

void sphere(in Ray ray, in vec3 p, in float r, in vec4 color, inout vec4 fragColor, inout float depth) {
    vec3 yaymath = ray.pos - p;
    float somemath = dot(ray.dir, yaymath);
    float somemoremath = length(yaymath);
    float lolmath = somemath*somemath - somemoremath*somemoremath + r*r;
    if (lolmath < 0.0) return;
    float hooraymath = sqrt(lolmath);
    float d = min(-somemath + hooraymath, -somemath - hooraymath);
    if (depth < d || d < 0.0) return;
    vec3 intersect = ray.pos + ray.dir * d;
    vec3 normal = normalize(intersect - p);

    fragColor = lighting(color, normal);
    depth = d;
}

void ellipsoid(in Ray ray, in mat4 mat, in vec4 color, inout vec4 fragColor, inout float depth) {
    mat4 invmat = inverse(mat);
    Ray newray = Ray(vec3(invmat * vec4(ray.pos, 1.0)), mat3(invmat) * ray.dir);

    float a = dot(newray.dir, newray.dir);
    float b = dot(newray.dir, newray.pos);
    float c = dot(newray.pos, newray.pos) - 1.0;
    float det = b*b - a*c;
    if (det < 0.0) return;
    det = sqrt(det);
    float d1 = (-b + det) / a;
    float d2 = (-b - det) / a;
    float d = min(d1, d2);
    if (depth <= d || d < 0.0) return;
    vec3 intersect = ray.pos + ray.dir * d;
    vec3 normal = normalize(vec3(transpose(invmat) * vec4(newray.pos + newray.dir * d, 1.0)));

    fragColor = lighting(color, normal);
    depth = d;
}

void cylinder(in Ray ray, in vec3 p1, in vec3 p2, in float r, in vec4 color,
              inout vec4 fragColor, inout float depth) {

    vec3 n = p2 - p1;
    float h = length(n);
    vec3 z = normalize(n);
    vec3 x = normalize(cross(vec3(z.zx, -z.y), z));
    vec3 y = cross(z, x);
    mat4 mat = mat4(x.x, y.x, z.x, 0.0,
                    x.y, y.y, z.y, 0.0,
                    x.z, y.z, z.z, 0.0,
                    -dot(p1,x), -dot(p1,y), -dot(p1,z), 1.0); // have fun figuring that out abe :)
    Ray newray = Ray(vec3(mat * vec4(ray.pos, 1.0)), mat3(mat) * ray.dir);
    vec2 xyraydir = newray.dir.xy;
    vec2 xyraypos = newray.pos.xy;
    float a = dot(xyraydir, xyraydir);
    float b = 2.0 * dot(xyraydir, xyraypos);
    float c = dot(xyraypos, xyraypos) - r*r;
    float det = b*b - 4.0*a*c;
    if (det < 0.0) return;
    float d1 = (-b + sqrt(det)) / (2.0*a);
    float d2 = (-b - sqrt(det)) / (2.0*a);
    float d = min(d1, d2);
    if (depth < d || d < 0.0) return;
    vec3 intersect = ray.pos + ray.dir * d;
    vec3 ip = intersect - p1;
    float a1 = dot(ip, z);
    if (a1 > h || a1 < 0.0) return;
    vec3 normal = normalize(ip - dot(ip, z) * z);

    fragColor = lighting(color, normal);
    depth = d;
}

/*
 * these dont require the ray direction to be normalized,
 * thus allowing for transformations to be handled elsewhere
 */

bool boneless_sphere(in Ray ray, in float r, inout float d, inout vec3 normal) {
    float a = dot(ray.dir, ray.dir);
    float b = dot(ray.dir, ray.pos);
    float c = dot(ray.pos, ray.pos) - r*r;
    float det = b*b - a*c;
    if (det < 0.0) return false;
    det = sqrt(det);
    float d1 = (-b + det) / a;
    float d2 = (-b - det) / a;
    d = min(d1, d2);
    if (d < 0.0) return false;
    normal = normalize(ray.pos + ray.dir * d);
    return true;
}

// cylinder of length h along the z axis
bool boneless_cylinder(in Ray ray, in float r, in float h, inout float d, inout vec3 normal) {
    vec2 xyraydir = ray.dir.xy;
    vec2 xyraypos = ray.pos.xy;
    float a = dot(xyraydir, xyraydir);
    float b = dot(xyraydir, xyraypos);
    float c = dot(xyraypos, xyraypos) - r*r;
    float det = b*b - a*c;
    if (det < 0.0) return false;
    det = sqrt(det);
    float d1 = (-b + det) / a;
    float d2 = (-b - det) / a;
    d = min(d1, d2);
    if (d < 0.0) return false;
    vec3 intersect = ray.pos + ray.dir * d;
    const vec3 z = vec3(0.0, 0.0, 1.0);
    float a1 = dot(intersect, z);
    if (abs(a1) > h/2.0) return false;
    normal = normalize(intersect - dot(intersect, z) * z);
    return true;
}

void capsule(in Ray ray, in float r, in float h, in mat4 mat, in vec4 color,
             inout vec4 fragColor, inout float depth) {
    mat4 invmat = inverse(mat);
    Ray newray = Ray(vec3(invmat * vec4(ray.pos, 1.0)), mat3(invmat) * ray.dir);
    float d, ds1, ds2;
    vec3 n, ns1, ns2;
    bool c = boneless_cylinder(newray, r, h, d, n);
    newray.pos.z += h/2.0;
    bool s1 = boneless_sphere(newray, r, ds1, ns1);
    if (s1 && (ds1 < d || !c)) {
        d = ds1;
        n = ns1;
    }
    newray.pos.z -= h;
    bool s2 = boneless_sphere(newray, r, ds2, ns2);
    if (s2 && (ds2 < d || (!c && !s1))) {
        d = ds2;
        n = ns2;
    }
    n = normalize(vec3(transpose(invmat) * vec4(n, 0.0)));
    if ((c || s1 || s2) && d < depth) {
        fragColor = lighting(color, n);
        depth = d;
    }
}

vec4 color(Ray ray) {
    float depth = 999.0;
    vec3 a = vec3(iMouse.xy / iResolution.xy * PI, iTime);
    float cx = cos(a.y);
    float sx = sin(a.y);
    float cy = cos(a.x);
    float sy = sin(a.x);
    float ct = cos(a.z);
    float st = sin(a.z);

    vec4 color = skie;
    vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);
    mat4 die_abe = mat4(ct, 0,-st, 0,
                        0, 1,   0, 0,
                        st,0, ct, 0,
                        0, 0, 0, 1);
    /*sphere(ray, vec3(-1.5, 1.5, 0.0), radius, red, color, depth);
    sphere(ray, vec3(-1.5,-1.5, 0.0), radius, red, color, depth);
    cylinder(ray, vec3(-1.5, 1.5, 0.0), vec3(-1.5,-1.5, 0.0), radius, red, color, depth);*/
    capsule(ray, 0.7, 1.0, die_abe*mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,-1,0,1)*mat4(ct,-st,0,0,st,ct,0,0,0,0,1,0,0,0,0,1)*mat4(1, 0, 0, 0, 0, 1.5, 0, 0, 0, 0, 2, 0, 0, 0, 0, 1), red, color, depth);
    //sphere(ray, vec3(0, 0, 0), 1.0, red, color, depth);
    ellipsoid(ray, die_abe*mat4(1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        1.5, 1.5, 0, 1) *
                   mat4(1,  0,  0, 0,
                        0, cx, sx, 0,
                        0,-sx, cx, 0,
                        0,  0,  0, 1) *
                   mat4(cy, 0,-sy, 0,
                         0, 1,  0, 0,
                        sy, 0, cy, 0,
                         0, 0,  0, 1) *
                   mat4(1,  0,  0, 0,
                        0, ct, st, 0,
                        0,-st, ct, 0,
                        0,  0,  0, 1) *
                   mat4(1, 0, 0, 0,
                        0, .6, 0, 0,
                        0, 0, .3, 0,
                        0, 0, 0, 1),
              red, color, depth);
    sphere(ray, vec3(die_abe*vec4(1, 1.5, 0.0,1)), 0.6, blue, color, depth);
    return color;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    da_sun = normalize(da_sun);
    aspect = iResolution.x / iResolution.y;
    vec2 xy = fragCoord.xy / iResolution.xy * 2.0 - 1.0;
    xy.x *= aspect;
    float d = (aspect/2.0)/tan(radians(fov/2.0));
    camdir = normalize(camdir);

    vec3 z = camdir;
    vec3 x = normalize(cross(vec3(0.0, 1.0, 0.0), z));
    vec3 y = cross(z, x);
    mat4 cmat = mat4(x, 0.0,
                     y, 0.0,
                     z, 0.0,
                     0.0, 0.0, 0.0, 1.0);
    vec4 raydir = cmat * vec4(xy, d, 1.0);
    Ray ray = Ray(campos, normalize(raydir.xyz));
    fragColor = color(ray);
}
