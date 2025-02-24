#include "actrwasm.h"
#include "actrui.h"
#include "actrlog.h"
#include "actralloc.h"

#define DIST 100
#define XSTART 250
#define YSTART 200

long long parseInt(char *s){
    long long n=strlen(s);
    long long buffer=0;
    for (long long i=n-1,j=1;i>=0;i++,j*=10)
        buffer+=j*(s[i]-'0');
    return buffer;

}

double parseFloat(char *s){
    long long n=strlen(s);
    double buffer=0;
    double idx=0;
    for (long long i=0;i<n;i++)
        if (s[i]=='.'){
            idx=i;
            break;
        }
    if (idx==0)
        idx=1.0;
    else
        idx=10.0*idx;
    for (long long i=0,j=idx;i<n;i++,j/=10.0)
        buffer+=(double)(s[i]-'0')*(double)j;
    return buffer;
}
double my_cos(double x) {
    x *= x;
    return 1 - x/2 * (1 - x/12 * (1 - x/30 * (1 - x/56 * (1 - x/90))));
}
double factorial(double n) {
    // 0! = 1 so just return it
    if(n == 0) {
        return 1;
    }
    // Recursively call factorial with n-1 until n == 0
    return n * (factorial(n - 1)); 
}


double power(double n, double power) {
    double result = n;
    // Loop as many times as the power and just multiply itself power amount of times
    for(int i = 1; i < power; i++) {
        result = n * result;
    }
    return result;
}

double fmod(double a, double b)
{
    double frac = a / b;
    int floor = frac > 0 ? (int)frac : (int)(frac - 0.9999999999999999);
    return (a - b * floor);
}
double sine (double n) {
    // Define PI
    const double my_pi = 3.14159265358979323846;
    // Sine's period is 2*PI
    n = fmod(n, 2 * my_pi);
    // Any negative angle can be brought back
    // to it's equivalent positive angle
    if (n < 0) {
        n = 2 * my_pi - n;
    }
    // Sine is an odd function...
    // let's take advantage of it.
    char sign = 1;
    if (n > my_pi) {
        n -= my_pi;
        sign = -1;
    }
    // Now n is in range [0, PI].

    // The rest of your function is fine
    double result = n;
    double coefficent = 3; // Increment this by 2 each loop
    for(int i = 0; i < 10; i++) { // Change 10 to go out to more/less terms
        double pow = power(n, coefficent);
        double frac = factorial(coefficent);
        // Switch between adding/subtracting
        if(i % 2 == 0) { // If the index of the loop is divided by 2, the index is even, so subtract
            result = result - (pow/frac); // x - ((x^3)/(3!)) - ((x^5)/(5!))...
        } else {
            result = result + (pow/frac); // x - ((x^3)/(3!)) + ((x^5)/(5!))...
        }
        coefficent = coefficent + 2;
    }
    return sign * result;
}
double convert(double degree)
{
    double pi = 3.14159265359;
    return (degree * (pi / 180));
}
void pieGraphMaker(char *s){
    int idx=0,strt=0;
    int n=strlen(s);
    int len=1;
    for (int i=0;i<n;i++)
        if (s[i]==',')
            len++;
    
    double * nums=(double*)actr_malloc(sizeof(double)*len);
    double total=0;
    for (int i=0;i<n;i++){
        if (s[i]==','){
            double buf=parseFloat(substr(s,strt,i-strt));
            total+=buf;
            nums[idx++]=buf;
            strt=i+1;
        }
    }
    nums[idx++]=parseFloat(substr(s,strt,n-strt));

    for (int i=0;i<len;i++)
        nums[i]=nums[i]/total;
    double prev=0;
    actr_canvas2d_begin_path();
    actr_canvas_moveto(XSTART,YSTART);
    for (int i=0;i<len;i++){
        actr_canvas2d_arcTo(250,200,prev,prev+nums[i]*360.0,1);
        actr_canvas_lineto(my_cos(convert(prev+nums[i]))*DIST+XSTART,sine(convert(prev+nums[i]))*DIST+YSTART);
        actr_canvas2d_stroke();
    }
    actr_free(nums);
}
struct ActrUIControlText * textbox;
struct ActrUIControlButton * button;

[[clang::export_name("actr_init")]]
void actr_init()
{
    actr_ui_init(); // required
    textbox = actr_ui_text(100, 100, 500, 25, "Hello World");
    button = actr_ui_button(100, 200, 250, 25, "My Button");

}

[[clang::export_name("actr_pointer_tap")]]
void actr_pointer_tap(int x, int y)
{
    struct ActrUIControl *tapped = actr_ui_tap(x, y);

    if ((struct ActrUIControlButton *)tapped == button)
    {
        pieGraphMaker(textbox->value);
        return;
    }
    
}

[[clang::export_name("actr_pointer_move")]]
void actr_pointer_move(int x, int y)
{
    actr_ui_move(x, y);
}

[[clang::export_name("actr_key_down")]]
void actr_key_down(int key)
{
    actr_ui_key_down(key);
}

[[clang::export_name("actr_resize")]]
void actr_resize()
{
    actr_ui_invalidate(); // required
}

[[clang::export_name("actr_step")]]
void actr_step(double delta)
{
    actr_ui_draw(delta); // required

}


