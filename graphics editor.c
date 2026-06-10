#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
 
#define ROWS 30
#define COLS 80
#define MAX_OBJECTS 100
#define CANVAS_CHAR '_'
#define DRAW_CHAR '*'
 
// ─── Object Types ───────────────────────────────────────────────────────────
typedef enum { CIRCLE, RECTANGLE, LINE, TRIANGLE } ObjectType;
 
typedef struct {
    ObjectType type;
    int visible;
    // params vary by type:
    // CIRCLE:    x,y = center; r = radius
    // RECTANGLE: x,y = top-left; r = width; extra = height
    // LINE:      x,y = start; r = x2; extra = y2
    // TRIANGLE:  x,y = apex; r = base_width; extra = height
    int x, y, r, extra;
} Object;
 
// ─── Canvas & Object Store ───────────────────────────────────────────────────
char canvas[ROWS][COLS];
Object objects[MAX_OBJECTS];
int obj_count = 0;
 
// ─── Canvas Utilities ────────────────────────────────────────────────────────
void init_canvas() {
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS; j++)
            canvas[i][j] = CANVAS_CHAR;
}
 
void display_canvas() {
    printf("\n");
    // top border
    printf("+");
    for (int j = 0; j < COLS; j++) printf("-");
    printf("+\n");
 
    for (int i = 0; i < ROWS; i++) {
        printf("|");
        for (int j = 0; j < COLS; j++)
            printf("%c", canvas[i][j]);
        printf("|\n");
    }
 
    // bottom border
    printf("+");
    for (int j = 0; j < COLS; j++) printf("-");
    printf("+\n");
}
 
void put_pixel(int row, int col) {
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
        canvas[row][col] = DRAW_CHAR;
}
 
void clear_pixel(int row, int col) {
    if (row >= 0 && row < COLS && col >= 0 && col < COLS)
        canvas[row][col] = CANVAS_CHAR;
}
 
// ─── Drawing Primitives ──────────────────────────────────────────────────────
 
// Midpoint circle algorithm      //Day 2
void draw_circle(int cx, int cy, int r) {
    int x = 0, y = r;
    int d = 1 - r;
    while (x <= y) {
        put_pixel(cy + y, cx + x);  put_pixel(cy + y, cx - x);
        put_pixel(cy - y, cx + x);  put_pixel(cy - y, cx - x);
        put_pixel(cy + x, cx + y);  put_pixel(cy + x, cx - y);
        put_pixel(cy - x, cx + y);  put_pixel(cy - x, cx - y);
        if (d < 0) d += 2 * x + 3;
        else { d += 2 * (x - y) + 5; y--; }
        x++;
    }
}
 
void draw_rectangle(int x, int y, int w, int h) {
    for (int j = x; j < x + w; j++) {
        put_pixel(y,     j);
        put_pixel(y+h-1, j);
    }
    for (int i = y; i < y + h; i++) {
        put_pixel(i, x);
        put_pixel(i, x+w-1);
    }
}
 
// Bresenham line algorithm
void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1-x0), dy = abs(y1-y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    while (1) {
        put_pixel(y0, x0);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}
 
// Isoceles triangle: apex at (cx, top_y), base_width, height
void draw_triangle(int cx, int top_y, int base_w, int h) {
    // apex
    put_pixel(top_y, cx);
    // two slanted sides
    int half = base_w / 2;
    draw_line(cx, top_y, cx - half, top_y + h - 1);
    draw_line(cx, top_y, cx + half, top_y + h - 1);
    // base
    draw_line(cx - half, top_y + h - 1, cx + half, top_y + h - 1);
}
 
// ─── Redraw All Visible Objects ───────────────────────────────────────────────
void redraw_all() {
    init_canvas();
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].visible) continue;
        Object *o = &objects[i];
        switch (o->type) {
            case CIRCLE:    draw_circle(o->x, o->y, o->r); break;
            case RECTANGLE: draw_rectangle(o->x, o->y, o->r, o->extra); break;
            case LINE:      draw_line(o->x, o->y, o->r, o->extra); break;
            case TRIANGLE:  draw_triangle(o->x, o->y, o->r, o->extra); break;
        }
    }
}
 
// ─── Add Object helpers ───────────────────────────────────────────────────────
int add_object(ObjectType t, int x, int y, int r, int extra) {
    if (obj_count >= MAX_OBJECTS) { printf("Object limit reached!\n"); return -1; }
    objects[obj_count] = (Object){ t, 1, x, y, r, extra };
    return obj_count++;
}
 
// ─── Menu Handlers ───────────────────────────────────────────────────────────
void menu_add() {  //day3
    printf("\nAdd Object:\n");
    printf("  1. Circle\n  2. Rectangle\n  3. Line\n  4. Triangle\n  0. Cancel\n");
    printf("Choice: ");
    int ch; scanf("%d", &ch);
    int x, y, r, w, h, x2, y2, bw;
    switch (ch) {
        case 1:
            printf("Center col (0-%d): ", COLS-1); scanf("%d", &x);
            printf("Center row (0-%d): ", ROWS-1); scanf("%d", &y);
            printf("Radius: "); scanf("%d", &r);
            add_object(CIRCLE, x, y, r, 0);
            printf("Circle added (id %d).\n", obj_count-1);
            break;
        case 2:
            printf("Top-left col: "); scanf("%d", &x);
            printf("Top-left row: "); scanf("%d", &y);
            printf("Width: ");  scanf("%d", &w);
            printf("Height: "); scanf("%d", &h);
            add_object(RECTANGLE, x, y, w, h);
            printf("Rectangle added (id %d).\n", obj_count-1);
            break;
        case 3:
            printf("Start col: "); scanf("%d", &x);
            printf("Start row: "); scanf("%d", &y);
            printf("End col: ");   scanf("%d", &x2);
            printf("End row: ");   scanf("%d", &y2);
            add_object(LINE, x, y, x2, y2);
            printf("Line added (id %d).\n", obj_count-1);
            break;
        case 4:
            printf("Apex col: ");      scanf("%d", &x);
            printf("Apex row: ");      scanf("%d", &y);
            printf("Base width: ");    scanf("%d", &bw);
            printf("Height: ");        scanf("%d", &h);
            add_object(TRIANGLE, x, y, bw, h);
            printf("Triangle added (id %d).\n", obj_count-1);
            break;
        default: return;
    }
    redraw_all();
    display_canvas();
}
 
void menu_delete() {
    if (obj_count == 0) { printf("No objects to delete.\n"); return; }
    printf("\nCurrent objects:\n");
    const char *names[] = {"Circle","Rectangle","Line","Triangle"};
    for (int i = 0; i < obj_count; i++) {
        Object *o = &objects[i];
        printf("  [%d] %s at (%d,%d) %s\n",
               i, names[o->type], o->x, o->y,
               o->visible ? "" : "[hidden]");
    }
    printf("Enter id to delete (-1 to cancel): ");
    int id; scanf("%d", &id);
    if (id < 0 || id >= obj_count) { printf("Cancelled.\n"); return; }
    objects[id].visible = 0;
    printf("Object %d deleted.\n", id);
    redraw_all();
    display_canvas();
}
 
void menu_modify() {
    if (obj_count == 0) { printf("No objects to modify.\n"); return; }
    printf("\nCurrent objects:\n");
    const char *names[] = {"Circle","Rectangle","Line","Triangle"};
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].visible) continue;
        Object *o = &objects[i];
        printf("  [%d] %s\n", i, names[o->type]);
    }
    printf("Enter id to modify (-1 to cancel): ");
    int id; scanf("%d", &id);
    if (id < 0 || id >= obj_count || !objects[id].visible) {
        printf("Cancelled.\n"); return;
    }
    Object *o = &objects[id];
    printf("New parameters for %s:\n", names[o->type]);
    int x, y, r, extra;
    switch (o->type) {
        case CIRCLE:
            printf("New center col: "); scanf("%d", &x);
            printf("New center row: "); scanf("%d", &y);
            printf("New radius: ");     scanf("%d", &r);
            o->x=x; o->y=y; o->r=r;
            break;
        case RECTANGLE:
            printf("New top-left col: "); scanf("%d", &x);
            printf("New top-left row: "); scanf("%d", &y);
            printf("New width: ");  scanf("%d", &r);
            printf("New height: "); scanf("%d", &extra);
            o->x=x; o->y=y; o->r=r; o->extra=extra;
            break;
        case LINE:
            printf("New start col: "); scanf("%d", &x);
            printf("New start row: "); scanf("%d", &y);
            printf("New end col: ");   scanf("%d", &r);
            printf("New end row: ");   scanf("%d", &extra);
            o->x=x; o->y=y; o->r=r; o->extra=extra;
            break;
        case TRIANGLE:
            printf("New apex col: ");   scanf("%d", &x);
            printf("New apex row: ");   scanf("%d", &y);
            printf("New base width: "); scanf("%d", &r);
            printf("New height: ");     scanf("%d", &extra);
            o->x=x; o->y=y; o->r=r; o->extra=extra;
            break;
    }
    printf("Object %d modified.\n", id);
    redraw_all();
    display_canvas();
}
 
void menu_list() {
    if (obj_count == 0) { printf("No objects.\n"); return; }
    const char *names[] = {"Circle","Rectangle","Line","Triangle"};
    printf("\n%-4s %-12s %-6s %-6s %-6s %-6s %-8s\n",
           "ID","Type","X","Y","P3","P4","Status");
    printf("--------------------------------------------------\n");
    for (int i = 0; i < obj_count; i++) {
        Object *o = &objects[i];
        printf("%-4d %-12s %-6d %-6d %-6d %-6d %-8s\n",
               i, names[o->type], o->x, o->y, o->r, o->extra,
               o->visible ? "visible" : "deleted");
    }
}
 
// ─── Main Menu ────────────────────────────────────────────────────────────────
int main() {
    init_canvas();
    printf("==============================================\n");
    printf("   2D Character Graphics Editor (C)          \n");
    printf("   Canvas: %d cols x %d rows                 \n", COLS, ROWS);
    printf("   Draw char: '*'   Background char: '_'     \n");
    printf("==============================================\n");
    display_canvas();
 
    int choice;
    do {
        printf("\n╔══════════════════════════════╗\n");
        printf("║        MAIN MENU             ║\n");
        printf("╠══════════════════════════════╣\n");
        printf("║  1. Add object               ║\n");
        printf("║  2. Delete object            ║\n");
        printf("║  3. Modify object            ║\n");
        printf("║  4. Display canvas           ║\n");
        printf("║  5. List objects             ║\n");
        printf("║  0. Exit                     ║\n");
        printf("╚══════════════════════════════╝\n");
        printf("Choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: menu_add();     break;
            case 2: menu_delete();  break;
            case 3: menu_modify();  break;
            case 4: display_canvas(); break;
            case 5: menu_list();    break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
    return 0;
}
