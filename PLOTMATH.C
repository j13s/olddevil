#define SCALAR_3D(a,b) \
 ((a)->x[0]*(b)->x[0]+(a)->x[1]*(b)->x[1]+(a)->x[2]*(b)->x[2])
#define SCALAR_2D(a,b) \
 ((a)->x[0]*(b)->x[0]+(a)->x[1]*(b)->x[1])
static inline struct point *ADD_3D(struct point *e,struct point *a,
 struct point *b)
 { (e)->x[0]=(a)->x[0]+(b)->x[0];
   (e)->x[1]=(a)->x[1]+(b)->x[1];
   (e)->x[2]=(a)->x[2]+(b)->x[2]; return e; }
static inline struct point *SUB_3D(struct point *e,struct point *a,
 struct point *b)
 { (e)->x[0]=(a)->x[0]-(b)->x[0];
   (e)->x[1]=(a)->x[1]-(b)->x[1];
   (e)->x[2]=(a)->x[2]-(b)->x[2]; return e; }
static inline struct point_2d *ADD_2D(struct point_2d *e,struct point_2d *a,
 struct point_2d *b)
 { (e)->x[0]=(a)->x[0]+(b)->x[0];
   (e)->x[1]=(a)->x[1]+(b)->x[1]; return e; }
static inline struct point_2d *SUB_2D(struct point_2d *e,struct point_2d *a,
 struct point_2d *b)
 { (e)->x[0]=(a)->x[0]-(b)->x[0];
   (e)->x[1]=(a)->x[1]-(b)->x[1]; return e; }
static inline float round(float x)
 { return x<0.0 ? floor(x+0.5) : ceil(x-0.5); }

