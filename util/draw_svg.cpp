#include <iostream>
#include <cstring>
#include <svg.h>

#include <opencv/highgui.h>

#include "device_cv_sim.hpp"

using namespace std;

class svg_render_state_t
{
public:
    svg_render_state_t( Device::Generic & tempdev ) : device(tempdev)
    {
        set_transform(1,0,0,0,1,0);
    }

    bool move_to( const xy & pt );
    bool cut_to(  const xy & pt );
    bool curve_to( const xy & pta, const xy & ptb, const xy & ptc, const xy & ptd );

    void set_transform( double a, double b, double c, double d, double e, double f );

    xy get_last_moved_to( void ){ return last_moved_to; }
private:
    double transform[3][3];
    xy last_moved_to;
    Device::Generic & device;
    xy apply_transform( const xy & pt );
};

bool svg_render_state_t::move_to( const xy & pt )
{
xy buf;
last_moved_to = pt;
buf = apply_transform(pt);
cout<<"    transform mov to:"<<buf.x<<','<<buf.y<<endl;
return device.move_to( buf );
}

/*
svg_render_state_t::svg_render_state_t( Device::Generic & tempdev )
{
memset( transform, 0x00, sizeof( transform ) );
transform[0][0] = 1;
transform[1][1] = 1;
transform[2][2] = 1;
device = tempdev;
}*/

void svg_render_state_t::set_transform( double a, double b, double c, double d, double e, double f )
{
transform[0][0] = a;
transform[0][1] = c;
transform[0][2] = e;
transform[1][0] = b;
transform[1][1] = d;
transform[1][2] = f;
transform[2][0] = 0;
transform[2][1] = 0;
transform[2][2] = 1;
}

xy svg_render_state_t::apply_transform( const xy & pt )
{
xy buf;
double scalar;

buf.x  = transform[0][0] * pt.x + transform[0][1] * pt.y + transform[0][2] * 1.0;
buf.y  = transform[1][0] * pt.x + transform[1][1] * pt.y + transform[1][2] * 1.0;
scalar = transform[2][0] * pt.x + transform[2][1] * pt.y + transform[2][2] * 1.0;

if( scalar == 0 )
    {
    buf.x = 0;
    buf.y = 0;
    }
else
    {
    buf.x /= scalar;
    buf.y /= scalar;
    }

buf.x /= 100;
buf.y /= 100;

return buf;
}

bool svg_render_state_t::cut_to( const xy & pt )
{
xy buf;

buf = apply_transform( pt );
cout<<"    transform cut to:"<<buf.x<<','<<buf.y<<endl;
return device.cut_to( buf );
}

static svg_status_t begin_group_callback( void * closure, double opacity )
{
//    cout<<"Begin group called with opacity="<<opacity<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t end_group_callback( void * closure, double opacity )
{
//    cout<<"End group called with opacity="<<opacity<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t begin_element_callback( void * ptr )
{
//    cout<<"Begin element callback called"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t end_element_callback( void * ptr )
{
//    cout<<"End element callback called"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t move_callback( void * ptr, double x, double y )
{
    xy pt;
    cout << "Moving to "<<x<<','<<y<<endl;

    pt.x = x;
    pt.y = y;

    ((svg_render_state_t*)ptr)->move_to( pt );
    return SVG_STATUS_SUCCESS;
}


static svg_status_t line_callback( void * ptr, double x, double y )
{
    xy point;

    cout << "Cutting to "<<x<<','<<y<<endl;

    point.x = x;
    point.y = y;

    ((svg_render_state_t*)ptr)->cut_to( point );
    return SVG_STATUS_SUCCESS;
}


static svg_status_t curve_callback( void * ptr, double x1, double y1, double x2, double y2, double x3, double y3 )
{
    cout <<"Doing a curve!"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t quadratic_curve_callback( void * ptr, double x1, double y1, double x2, double y2 )
{
    cout <<"Doing a quadratic curve"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t arc_callback( void * ptr, double rx, double ry, double x_axis_rotation, int large_arc_flag, int sweep_flag, double x, double y )
{
    cout<<"Doing an arc"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t close_path_callback( void * ptr )
{
    cout <<"Closing path"<<endl;

    ((svg_render_state_t*)ptr)->cut_to( ((svg_render_state_t*)ptr)->get_last_moved_to() );
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_color_callback( void * ptr, const svg_color_t * color )
{
//    cout<<"Setting color"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_stroke_width_callback( void * ptr, svg_length_t * width )
{
//    cout << "Setting stroke width " << endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_fill_opacity_callback( void * ptr, double fill_opacity )
{
//    cout <<"Should be setting fill opacity="<<fill_opacity<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_fill_paint_callback( void * ptr, const svg_paint_t * paint )
{
//    cout << "Ignoring fill paint"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_fill_rule_callback( void * ptr, const svg_fill_rule_t fill_rule )
{
//    cout << "Ignoring fill rule"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_viewport_dimension_callback( void * ptr, svg_length_t * width, svg_length_t * height )
{
    cout<<"Setting viewport dimensions"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t apply_view_box_callback( void * ptr, svg_view_box_t view_box, svg_length_t *width, svg_length_t * height )
{
    cout<<"Applying view box"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t transform_callback( void * ptr, double a, double b, double c, double d, double e, double f)
{
    cout <<"Some sort of transform"<<endl;
    cout <<"    a="<<a<<endl;
    cout <<"    b="<<b<<endl;
    cout <<"    c="<<c<<endl;
    cout <<"    d="<<d<<endl;
    cout <<"    e="<<e<<endl;
    cout <<"    f="<<f<<endl;
    ((svg_render_state_t*)ptr)->set_transform(a,b,c,d,e,f);
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_opacity_callback( void * ptr, double opacity )
{
//    cout <<"Setting opacity:"<<opacity<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_font_family_callback( void * ptr, const char * family )
{
//    cout <<"set font to:"<<family<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_font_size_callback( void * ptr, double sz )
{
//    cout<<"Set font size:"<<sz<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_font_style_callback( void * ptr, svg_font_style_t font_style )
{
//    cout<<"Set font style"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_font_weight_callback( void * ptr, unsigned int font_weight )
{
//    cout <<"Set font weight:"<<font_weight<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_stroke_dash_array_callback( void * ptr, double * dash_array, int num_dashes )
{
//    cout<<"Setting dash array to "<<num_dashes<<" dashes"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_stroke_dash_offset_callback( void * ptr, svg_length_t * offset )
{
//    cout <<"Setting dash offset"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_stroke_line_cap_callback( void * ptr, svg_stroke_line_cap_t line_cap )
{
//    cout <<"Setting stroke line cap"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_stroke_line_join_callback( void * ptr, svg_stroke_line_join_t line_join )
{
//    cout << "Setting stroke line join"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_stroke_miter_limit_callback( void * ptr, double limit )
{
//    cout <<"Setting stroke mitre limit to "<<limit<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_stroke_opacity_callback( void * ptr, double stroke_opacity )
{
//    cout <<"Setting stroke opacity to "<<stroke_opacity<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_stroke_paint_callback( void * ptr, const svg_paint_t * paint )
{
//    cout <<"Setting stroke paint"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t set_text_anchor_callback( void * ptr, svg_text_anchor_t text_anchor )
{
//    cout <<"Setting text anchor"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t render_line_callback( void * ptr, svg_length_t * x1, svg_length_t * y1, svg_length_t * x2, svg_length_t * y2 )
{
    cout<<"Rendering line from "<< x1->value<<','<< y1->value<<" to "<< x2->value<<','<< y2->value<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t render_path_callback( void * ptr )
{
    cout<<"Rendering path"<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t render_ellipse_callback( void * ptr, svg_length_t * cx, svg_length_t * cy, svg_length_t * rx, svg_length_t * ry )
{
//    static const double kappa = 4 * ( sqrt( 2 ) - 1 ) / 3;
    cout <<"Rendering ellipse"<<endl;
	cout <<"    cx=" << cx->value<<endl;
	cout <<"    cy=" << cy->value<<endl;
	cout <<"    rx=" << rx->value<<endl;
	cout <<"    ry=" << ry->value<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t render_rect_callback( void * ptr, svg_length_t * x, svg_length_t *y, svg_length_t*width, svg_length_t * height, svg_length_t * rx, svg_length_t * ry )
{
    xy point;

    cout <<"Rendering rect"<<endl;
	cout <<"         x=" << x->value<<endl;
	cout <<"         y=" << y->value<<endl;
	cout <<"        rx=" << rx->value<<endl;
	cout <<"        ry=" << ry->value<<endl;
	cout <<"     width=" << width->value<<endl;
	cout <<"    height=" << height->value<<endl;

//    point.x = x->value + ((svg_render_state_t*)ptr)->get_last_moved_to().x;
//    point.y = y->value + ((svg_render_state_t*)ptr)->get_last_moved_to().y;
//    point.x = ((svg_render_state_t*)ptr)->get_last_moved_to().x;
//    point.y = ((svg_render_state_t*)ptr)->get_last_moved_to().y;

    point.x = x->value;
    point.y = y->value;
    ((svg_render_state_t*)ptr)->move_to( point );


    point.x += width->value;
    ((svg_render_state_t*)ptr)->cut_to( point );

    point.y += height->value;
    ((svg_render_state_t*)ptr)->cut_to( point );

    point.x -= width->value;
    ((svg_render_state_t*)ptr)->cut_to( point );

    point.y -= height->value;
    ((svg_render_state_t*)ptr)->cut_to( point );

    return SVG_STATUS_SUCCESS;
}


static svg_status_t render_text_callback( void * ptr, svg_length_t * x, svg_length_t * y, const char * utf8 )
{
    cout<<"Rendering text:"<<utf8<<endl;
    return SVG_STATUS_SUCCESS;
}


static svg_status_t render_image_callback( void * ptr, unsigned char * data, unsigned int data_width, unsigned int data_height, svg_length_t *x, svg_length_t * y, svg_length_t * width, svg_length_t * height )
{
    cout<<"Rendering image"<<endl;
    return SVG_STATUS_SUCCESS;
}


int main(int numArgs, char * args[] )
{
    svg_t * svg;
    svg_length_t width;
    svg_length_t height;
    svg_render_engine_t engine;

    if( numArgs != 3 )
    {
        cout<<"Usage: "<<args[0]<<" svgfile.svg iodevice"<<endl;
        return 4;
    }

    Device::CV_sim c( args[2] );
    c.stop();
    c.start();

    svg_render_state_t state(c);

    //For debugging
    memset( (void*)&engine, 0xAD, sizeof( engine ) );

    engine.render_image           = render_image_callback;
    engine.render_text            = render_text_callback;
    engine.render_ellipse         = render_ellipse_callback;
    engine.render_path            = render_path_callback;
    engine.render_line            = render_line_callback;
    engine.render_rect            = render_rect_callback;

    engine.set_text_anchor        = set_text_anchor_callback;
    engine.set_stroke_width       = set_stroke_width_callback;
    engine.set_stroke_opacity     = set_stroke_opacity_callback;
    engine.set_stroke_paint       = set_stroke_paint_callback;
    engine.set_stroke_dash_array  = set_stroke_dash_array_callback;
    engine.set_stroke_dash_offset = set_stroke_dash_offset_callback;
    engine.set_stroke_line_cap    = set_stroke_line_cap_callback;
    engine.set_stroke_line_join   = set_stroke_line_join_callback;
    engine.set_stroke_miter_limit = set_stroke_miter_limit_callback;
    engine.set_font_family        = set_font_family_callback;
    engine.set_font_size          = set_font_size_callback;
    engine.set_font_style         = set_font_style_callback;
    engine.set_font_weight        = set_font_weight_callback;
    engine.set_opacity            = set_opacity_callback;
    engine.transform              = transform_callback;
    engine.apply_view_box         = apply_view_box_callback;
    engine.set_viewport_dimension = set_viewport_dimension_callback;
    engine.set_fill_paint         = set_fill_paint_callback;
    engine.set_fill_rule          = set_fill_rule_callback;
    engine.set_fill_opacity       = set_fill_opacity_callback;
    engine.set_color              = set_color_callback;
    engine.begin_group            = begin_group_callback;
    engine.begin_element          = begin_element_callback;
    engine.end_element            = end_element_callback;
    engine.end_group              = end_group_callback;
    engine.move_to                = move_callback;
    engine.line_to                = line_callback;
    engine.curve_to               = curve_callback;
    engine.quadratic_curve_to     = quadratic_curve_callback;
    engine.arc_to                 = arc_callback;
    engine.close_path             = close_path_callback;

    cout << "svg create returned: " << svg_create( &svg ) << endl;
    cout << "svg_parse  returned: " << svg_parse( svg, args[1] ) << endl;

    svg_get_size( svg, &width, &height );
    cout << "SVG: "<< width.value << "x" << height.value << endl;

    cout << "svg_render returned: " << svg_render( svg, &engine, (void*)&state ) << endl;

    svg_get_size( svg, &width, &height );
    cout << "SVG: "<< width.value << "x" << height.value << endl;

    cout << "svg dstry  returned: " << svg_destroy( svg ) << endl;

    IplImage * image = c.get_image();
    cvNamedWindow("cutter");
    cvShowImage("cutter", image );
    cvWaitKey(0);
    cvReleaseImage( &image );

    c.stop();
}
