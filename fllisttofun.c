#include "fllisttofun.h"

void C74_EXPORT main() {

	t_class *c;

	c = class_new("fllisttofun",(method)fl_ftom_new, (method)NULL,sizeof(t_fl_ftom), 0, 0);
	class_addmethod(c, (method)fl_ftom_assist,"assist", A_CANT, 0);
	class_addmethod(c,(method)fl_ftom_entero, "int", A_LONG, 0);
	class_addmethod(c,(method)fl_ftom_float, "float", A_LONG, 0);
	class_addmethod(c,(method)fl_ftom_list, "list", A_GIMME, 0);
	class_addmethod(c, (method)fl_ftom_mode, "mode", A_GIMME, 0);
	class_addmethod(c, (method)fl_ftom_listdump, "listdump", A_GIMME, 0);

	class_register(CLASS_BOX, c);
	fl_ftom_class = c; 
}

void *fl_ftom_new(t_symbol *s, short argc, t_atom *argv) 
{
	t_fl_ftom *x = (t_fl_ftom *)object_alloc(fl_ftom_class);

	x->m_outlet = outlet_new((t_object *)x, NULL);

	x->mode = DFLT_LINEAL;
	x->listdump = DFLT_LISTDUMP;

	return x;
}

void fl_ftom_assist(t_fl_ftom *x, void *b, long msg, long arg, char *dst)
{
	if (msg == ASSIST_INLET) {										
		switch (arg) {
		case I_INPUT: 
			sprintf(dst, "function list"); 
			break;
		}
	}
	else if (msg == ASSIST_OUTLET) {    
		switch (arg) {
		case O_OUTPUT: 
			sprintf(dst, "messages for function"); 
			break;
		}
	}
}

void fl_ftom_entero(t_fl_ftom *x, long n) {}

void fl_ftom_float(t_fl_ftom *x, double f) {}

void fl_ftom_list(t_fl_ftom *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	short brkpt_size = x->mode ? 3 : 2; //mode 0:linear (f,f) //mode 1:curve (f,f,f)
	short mode = x->mode;
	short format = x->listdump; //0:line-curve format (y,dx)(y,dx,c)  //1:listdump format (x,y)(x,y,c)
	double domain = 0.;
	double range_i = 0.;
	double range_f = 0.;
	long n_brkpt = 0;

	t_atom atom_out[2];
	atom_setfloat(atom_out, 0.);
	atom_setfloat(atom_out + 1, 0.);

	if (ac % brkpt_size != 0) {
		object_error((t_object *)x, "list size must be multiple of %d", brkpt_size);
		return;
	}
	
	n_brkpt = ac / brkpt_size;
	range_i = range_f = (double)atom_getfloat(ap + 1);
	if (format) { domain = (double)atom_getfloat(ap + ac - brkpt_size); }
	for (long i = 0; i < n_brkpt; i++) {
		long j = brkpt_size * i;
		range_i = (double)MIN(range_i, (double)atom_getfloat(ap + j + format));
		range_f = (double)MAX(range_f, (double)atom_getfloat(ap + j + format));
		if (!format) { domain += (double)atom_getfloat(ap + j + 1); } 
	}
	
	//clear
	outlet_anything(x->m_outlet, gensym("clear"), 0L, NULL);
	//mode
	atom_setlong(atom_out, mode);
	outlet_anything(x->m_outlet, gensym("mode"), 1, atom_out);
	//domain
	atom_setfloat(atom_out, domain);
	outlet_anything(x->m_outlet, gensym("setdomain"), 1, atom_out);
	//range	
	atom_setfloat(atom_out, range_i);
	atom_setfloat(atom_out + 1, range_f);
	outlet_anything(x->m_outlet, gensym("setrange"), 2, atom_out);
	//breakpoints //0:line-curve format (y,dx)(y,dx,c)  //1:listdump format (x,y)(x,y,c)
	if (format) {
		for (long i = 0; i < n_brkpt; i++) {
			long j = brkpt_size * i;
			atom_setfloat(atom_out, (double)atom_getfloat(ap + j)); //x
			atom_setfloat(atom_out + 1, (double)atom_getfloat(ap + j + 1)); //y
			outlet_list(x->m_outlet, NULL, 2, atom_out);
		}
	}
	else{
		double x_accum = 0.0;
		for (long i = 0; i < n_brkpt; i++) {
			long j = brkpt_size * i;
			x_accum += (double)atom_getfloat(ap + j + 1);
			atom_setfloat(atom_out, x_accum); //x
			atom_setfloat(atom_out + 1, (double)atom_getfloat(ap + j)); //y
			outlet_list(x->m_outlet, NULL, 2, atom_out);
		}
	}
	//curves	
	if (brkpt_size == 3) {
		for (long i = 1; i < n_brkpt; i++) {
			long j = brkpt_size * i;
			atom_setlong(atom_out, i);
			atom_setfloat(atom_out + 1, (double)atom_getfloat(ap + j + 2));
			outlet_anything(x->m_outlet, gensym("setcurve"), 2, atom_out);
		}
	}
}

void fl_ftom_mode(t_fl_ftom *x, t_symbol *s, long argc, t_atom *argv)
{
	short mode = 0;
	long ac = argc;
	t_atom *av = argv;
	
	if (!ac) { object_error((t_object *)x, "must have an argument"); return; }
	if (atom_gettype(av) != A_LONG && atom_gettype(av) != A_FLOAT) { object_error((t_object *)x, "argument must be a number"); return; }
	mode = (short)atom_getlong(av);

	if (mode < 0 || mode > 1) { object_error((t_object*)x, "argument must be a 0 or 1"); return; }
	x->mode = mode;
}

void fl_ftom_listdump(t_fl_ftom* x, t_symbol* s, long argc, t_atom* argv)
{
	short listdump = 0;
	long ac = argc;
	t_atom* av = argv;

	if (!ac) { object_error((t_object*)x, "must have an argument"); return; }
	if (atom_gettype(av) != A_LONG && atom_gettype(av) != A_FLOAT) { object_error((t_object*)x, "argument must be a number"); return; }
	listdump = (short)atom_getlong(av);

	if (listdump < 0 || listdump > 1) { object_error((t_object*)x, "argument must be a 0 or 1"); return; }
	x->listdump = listdump;
}