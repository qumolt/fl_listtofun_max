#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"  
#include <math.h>

#define DFLT_LINEAL 0	//mode 0 lineal
#define DFLT_LISTDUMP 0	//line format

//#define MIN(a,b) ((a) < (b) ? (a) : (b))
//#define MAX(a,b) ((a) > (b) ? (a) : (b))

enum INLETS { I_INPUT, NUM_INLETS };
enum OUTLETS { O_OUTPUT, NUM_OUTLETS };

typedef struct {

	t_object obj;
	
	short mode;
	short listdump;

	void *m_outlet;

} t_fl_ftom;

void *fl_ftom_new(t_symbol *s, short argc, t_atom *argv);
void fl_ftom_assist(t_fl_ftom *x, void *b, long msg, long arg, char *dst);

void fl_ftom_entero(t_fl_ftom *x, long n);
void fl_ftom_float(t_fl_ftom *x, double f); 
void fl_ftom_list(t_fl_ftom *x, t_symbol *s, long argc, t_atom *argv);
void fl_ftom_mode(t_fl_ftom *x, t_symbol *s, long argc, t_atom *argv);
void fl_ftom_listdump(t_fl_ftom *x, t_symbol *s, long argc, t_atom *argv);

static t_class *fl_ftom_class;