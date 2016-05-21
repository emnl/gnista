#include <ruby.h>
#include <sparkey/sparkey.h>

typedef uint8_t bool;
#define true 1
#define false 0


typedef struct instance_logwriter {
	sparkey_logwriter *logwriter;
	bool open;
} instance_logwriter;

typedef struct instance_logreader {
	sparkey_logreader *logreader;
	bool open;
} instance_logreader;

typedef struct instance_hashreader {
	sparkey_hashreader *hashreader;
	bool open;
} instance_hashreader;

/********************************************************************************/
/**************** ERROR HANDLING ************************************************/

VALUE GnistaException = Qnil;

static void raise_sparkey(sparkey_returncode returncode) {
	const char *error_msg = sparkey_errstring(returncode);
	rb_raise(GnistaException, "%s", error_msg);
}

static void check_open(bool open) {
	if (!open) {
		rb_raise(GnistaException, "Closed");
	}
}

/********************************************************************************/
/**************** LOGWRITER *****************************************************/

static void dealloc_logwriter(void *p) {
	instance_logwriter *i_logwriter = p;

	if (i_logwriter->open) {
		sparkey_returncode returncode;
		returncode = sparkey_logwriter_close(&i_logwriter->logwriter);

		if (returncode != SPARKEY_SUCCESS) {
			raise_sparkey(returncode);
		}
	}

	free(i_logwriter);
}

static VALUE alloc_logwriter(VALUE klass) {
	instance_logwriter *i_logwriter = ALLOC(instance_logwriter);
	i_logwriter->open = false;
	return Data_Wrap_Struct(klass, 0, dealloc_logwriter, i_logwriter);
}

static instance_logwriter* get_logwriter(VALUE self) {
	instance_logwriter *i_logwriter;
	Data_Get_Struct(self, instance_logwriter, i_logwriter);
	return i_logwriter;
}

static VALUE method_logwriter_initialize(VALUE self, VALUE args) {
	sparkey_returncode returncode;
	instance_logwriter *i_logwriter = get_logwriter(self);
	int len = RARRAY_LEN(args);

	if (len > 2 || len == 0) {
		rb_raise(rb_eArgError, "Wrong number of arguments");
	}

	Check_Type(rb_ary_entry(args, 0), T_STRING);

	if (len == 2) {
		if (TYPE(rb_ary_entry(args, 1)) == T_SYMBOL && (rb_to_id(rb_ary_entry(args, 1)) == rb_intern("append"))) {
			returncode = sparkey_logwriter_append(&i_logwriter->logwriter, RSTRING_PTR(rb_ary_entry(args, 0)));
		} else if (TYPE(rb_ary_entry(args, 1)) == T_FIXNUM) {
			returncode = sparkey_logwriter_create(&i_logwriter->logwriter, RSTRING_PTR(rb_ary_entry(args, 0)), SPARKEY_COMPRESSION_SNAPPY, NUM2INT(rb_ary_entry(args, 1)));
		} else {
			rb_raise(rb_eArgError, "Invalid arguments");
		}
	} else {
		returncode = sparkey_logwriter_create(&i_logwriter->logwriter, RSTRING_PTR(rb_ary_entry(args, 0)), SPARKEY_COMPRESSION_NONE, 0);
	}

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	} else {
		i_logwriter->open = true;
		rb_iv_set(self, "@logpath", rb_ary_entry(args, 0));
	}

	return self;
}

static VALUE method_logwriter_close(VALUE self) {
	sparkey_returncode returncode;
	instance_logwriter *i_logwriter = get_logwriter(self);
	check_open(i_logwriter->open);

	returncode = sparkey_logwriter_close(&i_logwriter->logwriter);

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	}

	i_logwriter->open = false;

	return Qnil;
}

static VALUE method_logwriter_put(VALUE self, VALUE key, VALUE val) {
	sparkey_returncode returncode;
	instance_logwriter *i_logwriter = get_logwriter(self);
	check_open(i_logwriter->open);
	Check_Type(key, T_STRING);
	Check_Type(val, T_STRING);

	returncode = sparkey_logwriter_put(i_logwriter->logwriter, RSTRING_LEN(key), (uint8_t*)RSTRING_PTR(key), RSTRING_LEN(val), (uint8_t*)RSTRING_PTR(val));

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	}

	return Qnil;
}

static VALUE method_logwriter_delete(VALUE self, VALUE key) {
	sparkey_returncode returncode;
	instance_logwriter *i_logwriter = get_logwriter(self);
	check_open(i_logwriter->open);
	Check_Type(key, T_STRING);

	returncode = sparkey_logwriter_delete(i_logwriter->logwriter, RSTRING_LEN(key), (uint8_t*)RSTRING_PTR(key));

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	}

	return Qnil;
}

static VALUE method_logwriter_flush(VALUE self) {
	sparkey_returncode returncode;
	instance_logwriter *i_logwriter = get_logwriter(self);
	check_open(i_logwriter->open);

	returncode = sparkey_logwriter_flush(i_logwriter->logwriter);

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	}

	return Qnil;
}

static VALUE method_logwriter_open(VALUE self) {
	instance_logwriter *i_logwriter = get_logwriter(self);

	if (i_logwriter->open) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}

/********************************************************************************/
/**************** LOGREADER *****************************************************/

static void dealloc_logreader(void *p) {
	instance_logreader *i_logreader = p;

	if (i_logreader->open) {
		sparkey_logreader_close(&i_logreader->logreader);
	}

	free(i_logreader);
}

static VALUE alloc_logreader(VALUE klass) {
	instance_logreader *i_logreader = ALLOC(instance_logreader);
	i_logreader->open = false;
	return Data_Wrap_Struct(klass, 0, dealloc_logreader, i_logreader);
}

static instance_logreader* get_logreader(VALUE self) {
	instance_logreader *i_logreader;
	Data_Get_Struct(self, instance_logreader, i_logreader);
	return i_logreader;
}

static VALUE method_logreader_initialize(VALUE self, VALUE filename) {
	sparkey_returncode returncode;
	instance_logreader *i_logreader = get_logreader(self);
	Check_Type(filename, T_STRING);

	returncode = sparkey_logreader_open(&i_logreader->logreader, RSTRING_PTR(filename));

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	} else {
		i_logreader->open = true;
		rb_iv_set(self, "@logpath", filename);
	}

	return self;
}

static VALUE method_logreader_close(VALUE self) {
	instance_logreader *i_logreader = get_logreader(self);
	check_open(i_logreader->open);
	sparkey_logreader_close(&i_logreader->logreader);
	i_logreader->open = false;

	return Qnil;
}

static VALUE method_logreader_each(VALUE self) {
	sparkey_returncode returncode;
	sparkey_logiter *logiter;
	instance_logreader *i_logreader = get_logreader(self);
	check_open(i_logreader->open);

	if (!rb_block_given_p()) {
		return rb_funcall(self, rb_intern("to_enum"), 0);
	}

	returncode = sparkey_logiter_create(&logiter, i_logreader->logreader);

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	}

	VALUE put_sym = ID2SYM(rb_intern("put"));
	VALUE del_sym = ID2SYM(rb_intern("delete"));
	uint8_t *keybuf = malloc(sparkey_logreader_maxkeylen(i_logreader->logreader));
	uint8_t *valuebuf = malloc(sparkey_logreader_maxvaluelen(i_logreader->logreader));
	while (i_logreader->open) {
		returncode = sparkey_logiter_next(logiter, i_logreader->logreader);

		if (sparkey_logiter_state(logiter) != SPARKEY_ITER_ACTIVE) {
			break;
		}

		uint64_t wanted_keylen = sparkey_logiter_keylen(logiter);
		uint64_t actual_keylen;
		returncode = sparkey_logiter_fill_key(logiter, i_logreader->logreader, wanted_keylen, keybuf, &actual_keylen);

		if (returncode != SPARKEY_SUCCESS) {
			free(keybuf);
			free(valuebuf);
			raise_sparkey(returncode);
		} else if (wanted_keylen != actual_keylen) {
			free(keybuf);
			free(valuebuf);
			rb_raise(GnistaException, "Corrupted read in logreader.");
		}

		if (sparkey_logiter_type(logiter) == SPARKEY_ENTRY_PUT) {
			uint64_t wanted_valuelen = sparkey_logiter_valuelen(logiter);
			uint64_t actual_valuelen;
			returncode = sparkey_logiter_fill_value(logiter, i_logreader->logreader, wanted_valuelen, valuebuf, &actual_valuelen);

			if (returncode != SPARKEY_SUCCESS) {
				free(keybuf);
				free(valuebuf);
				raise_sparkey(returncode);
			} else if (wanted_valuelen != actual_valuelen) {
				free(keybuf);
				free(valuebuf);
				rb_raise(GnistaException, "Corrupted read in logreader.");
			}

			rb_yield_values(3, rb_str_new((char *)keybuf, actual_keylen), rb_str_new((char *)valuebuf, actual_valuelen), put_sym);
		} else {
			rb_yield_values(3, rb_str_new((char *)keybuf, actual_keylen), Qnil, del_sym);
		}
	}

	free(keybuf);
	free(valuebuf);
	sparkey_logiter_close(&logiter);

	check_open(i_logreader->open);

	return self;
}

static VALUE method_logreader_open(VALUE self) {
	instance_logreader *i_logreader = get_logreader(self);

	if (i_logreader->open) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}

static VALUE method_logreader_maxkeylen(VALUE self) {
	instance_logreader *i_logreader = get_logreader(self);
	uint64_t maxkeylen;
	check_open(i_logreader->open);

	maxkeylen = sparkey_logreader_maxkeylen(i_logreader->logreader);

	return INT2NUM(maxkeylen);
}

static VALUE method_logreader_maxvaluelen(VALUE self) {
	instance_logreader *i_logreader = get_logreader(self);
	uint64_t maxvaluelen;
	check_open(i_logreader->open);

	maxvaluelen = sparkey_logreader_maxvaluelen(i_logreader->logreader);

	return INT2NUM(maxvaluelen);
}

/********************************************************************************/
/**************** HASH **********************************************************/

static void dealloc_hashreader(void *p) {
	instance_hashreader *i_hashreader = p;

	if (i_hashreader->open) {
		sparkey_hash_close(&i_hashreader->hashreader);
	}

	free(i_hashreader);
}

static VALUE alloc_hashreader(VALUE klass) {
	instance_hashreader *i_hashreader = ALLOC(instance_hashreader);
	i_hashreader->open = false;
	return Data_Wrap_Struct(klass, 0, dealloc_hashreader, i_hashreader);
}

static instance_hashreader* get_hashreader(VALUE self) {
	instance_hashreader *i_hashreader;
	Data_Get_Struct(self, instance_hashreader, i_hashreader);
	return i_hashreader;
}

static VALUE method_hash_write(VALUE klass, VALUE args) {
	sparkey_returncode returncode;
	int len = RARRAY_LEN(args);
	int hash_size = 0;

	if (len > 3 || len < 2) {
		rb_raise(rb_eArgError, "Wrong number of arguments");
	}

	Check_Type(rb_ary_entry(args, 0), T_STRING);
	Check_Type(rb_ary_entry(args, 1), T_STRING);

	if (len == 3) {
		Check_Type(rb_ary_entry(args, 2), T_FIXNUM);
		hash_size = NUM2INT(rb_ary_entry(args, 2));
	}

	returncode = sparkey_hash_write(RSTRING_PTR(rb_ary_entry(args, 0)), RSTRING_PTR(rb_ary_entry(args, 1)), hash_size);

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	}

	return Qnil;
}

static VALUE method_hash_initialize(VALUE self, VALUE hash_filename, VALUE log_filename) {
	sparkey_returncode returncode;
	instance_hashreader *i_hashreader = get_hashreader(self);

	Check_Type(hash_filename, T_STRING);
	Check_Type(log_filename, T_STRING);

	returncode = sparkey_hash_open(&i_hashreader->hashreader, RSTRING_PTR(hash_filename), RSTRING_PTR(log_filename));

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	} else {
		i_hashreader->open = true;
		rb_iv_set(self, "@hashpath", hash_filename);
		rb_iv_set(self, "@logpath", log_filename);
	}

	return Qnil;
}

static VALUE method_hash_close(VALUE self) {
	instance_hashreader *i_hashreader = get_hashreader(self);
	check_open(i_hashreader->open);
	sparkey_hash_close(&i_hashreader->hashreader);
	i_hashreader->open = false;

	return Qnil;
}

static VALUE method_hash_each(VALUE self) {
	sparkey_returncode returncode;
	instance_hashreader *i_hashreader = get_hashreader(self);
	sparkey_logiter *logiter;
	check_open(i_hashreader->open);

	if (!rb_block_given_p()) {
		return rb_funcall(self, rb_intern("to_enum"), 0);
	}

	sparkey_logreader *logreader = sparkey_hash_getreader(i_hashreader->hashreader);
	returncode = sparkey_logiter_create(&logiter, logreader);

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	}

	uint8_t *keybuf = malloc(sparkey_logreader_maxkeylen(logreader));
	uint8_t *valuebuf = malloc(sparkey_logreader_maxvaluelen(logreader));
	while (i_hashreader->open) {
		returncode = sparkey_logiter_hashnext(logiter, i_hashreader->hashreader);

		if (sparkey_logiter_state(logiter) != SPARKEY_ITER_ACTIVE) {
			break;
		}

		uint64_t wanted_keylen = sparkey_logiter_keylen(logiter);
		uint64_t actual_keylen;
		returncode = sparkey_logiter_fill_key(logiter, sparkey_hash_getreader(i_hashreader->hashreader), wanted_keylen, keybuf, &actual_keylen);

		if (returncode != SPARKEY_SUCCESS) {
			free(keybuf);
			free(valuebuf);
			raise_sparkey(returncode);
		} else if (wanted_keylen != actual_keylen) {
			free(keybuf);
			free(valuebuf);
			rb_raise(GnistaException, "Corrupt entry in logreader.");
		}

		uint64_t wanted_valuelen = sparkey_logiter_valuelen(logiter);
		uint64_t actual_valuelen;
		returncode = sparkey_logiter_fill_value(logiter, sparkey_hash_getreader(i_hashreader->hashreader), wanted_valuelen, valuebuf, &actual_valuelen);

		if (returncode != SPARKEY_SUCCESS) {
			free(keybuf);
			free(valuebuf);
			raise_sparkey(returncode);
		} else if (wanted_valuelen != actual_valuelen) {
			free(keybuf);
			free(valuebuf);
			rb_raise(GnistaException, "Corrupt entry in logreader.");
		}

		rb_yield_values(2, rb_str_new((char *)keybuf, actual_keylen), rb_str_new((char *)valuebuf, actual_valuelen));
	}

	free(keybuf);
	free(valuebuf);
	sparkey_logiter_close(&logiter);

	check_open(i_hashreader->open);

	return self;
}

static VALUE method_hash_get(VALUE self, VALUE key) {
	sparkey_returncode returncode;
	instance_hashreader *i_hashreader = get_hashreader(self);
	sparkey_logiter *logiter;
	check_open(i_hashreader->open);
	Check_Type(key, T_STRING);

	returncode = sparkey_logiter_create(&logiter, sparkey_hash_getreader(i_hashreader->hashreader));

	if (returncode != SPARKEY_SUCCESS) {
		raise_sparkey(returncode);
	}

	key = StringValue(key);

	returncode = sparkey_hash_get(i_hashreader->hashreader, (uint8_t*)RSTRING_PTR(key), RSTRING_LEN(key), logiter);

	if (sparkey_logiter_state(logiter) != SPARKEY_ITER_ACTIVE) {
		sparkey_logiter_close(&logiter);
		return Qnil;
	}

	uint64_t wanted_valuelen = sparkey_logiter_valuelen(logiter);
	uint8_t *valuebuf = malloc(wanted_valuelen);
	uint64_t actual_valuelen;
	returncode = sparkey_logiter_fill_value(logiter, sparkey_hash_getreader(i_hashreader->hashreader), wanted_valuelen, valuebuf, &actual_valuelen);
	sparkey_logiter_close(&logiter);

	if (returncode != SPARKEY_SUCCESS) {
		free(valuebuf);
		raise_sparkey(returncode);
	} else if (wanted_valuelen != actual_valuelen) {
		free(valuebuf);
		rb_raise(GnistaException, "Corrupt entry in hash.");
	}

	VALUE v = rb_str_new((char *)valuebuf, actual_valuelen);
	free(valuebuf);
	return v;
}

static VALUE method_hash_maxkeylen(VALUE self) {
	instance_hashreader *i_hashreader = get_hashreader(self);
	uint64_t maxkeylen;
	check_open(i_hashreader->open);

	maxkeylen = sparkey_logreader_maxkeylen(sparkey_hash_getreader(i_hashreader->hashreader));

	return INT2NUM(maxkeylen);
}

static VALUE method_hash_maxvaluelen(VALUE self) {
	instance_hashreader *i_hashreader = get_hashreader(self);
	uint64_t maxvaluelen;
	check_open(i_hashreader->open);

	maxvaluelen = sparkey_logreader_maxvaluelen(sparkey_hash_getreader(i_hashreader->hashreader));

	return INT2NUM(maxvaluelen);
}

static VALUE method_hash_length(VALUE self) {
	instance_hashreader *i_hashreader = get_hashreader(self);
	uint64_t numentries;
	check_open(i_hashreader->open);

	numentries = sparkey_hash_numentries(i_hashreader->hashreader);

	return INT2NUM(numentries);
}

static VALUE method_hash_collisions(VALUE self) {
	instance_hashreader *i_hashreader = get_hashreader(self);
	uint64_t collisions;
	check_open(i_hashreader->open);

	collisions = sparkey_hash_numcollisions(i_hashreader->hashreader);

	return INT2NUM(collisions);
}

static VALUE method_hash_open(VALUE self) {
	instance_hashreader *i_hashreader = get_hashreader(self);

	if (i_hashreader->open) {
		return Qtrue;
	} else {
		return Qfalse;
	}
}

/********************************************************************************/
/**************** INITIALIZE GNISTA *********************************************/

void Init_gnista() {
	VALUE Gnista = rb_define_module("Gnista");
	VALUE Logwriter = rb_define_class_under(Gnista, "Logwriter", rb_cObject);
	VALUE Logreader = rb_define_class_under(Gnista, "Logreader", rb_cObject);
	VALUE Hash = rb_define_class_under(Gnista, "Hash", rb_cObject);

	rb_define_alloc_func(Logwriter, alloc_logwriter);
	rb_define_method(Logwriter, "initialize", method_logwriter_initialize, -2);
	rb_define_method(Logwriter, "close", method_logwriter_close, 0);
	rb_define_method(Logwriter, "put", method_logwriter_put, 2);
	rb_define_method(Logwriter, "delete", method_logwriter_delete, 1);
	rb_define_method(Logwriter, "flush", method_logwriter_flush, 0);
	rb_define_method(Logwriter, "open?", method_logwriter_open, 0);

	rb_define_alloc_func(Logreader, alloc_logreader);
	rb_define_method(Logreader, "initialize", method_logreader_initialize, 1);
	rb_define_method(Logreader, "close", method_logreader_close, 0);
	rb_define_method(Logreader, "each", method_logreader_each, 0);
	rb_define_method(Logreader, "open?", method_logreader_open, 0);
	rb_define_method(Logreader, "maxkeylen", method_logreader_maxkeylen, 0);
	rb_define_method(Logreader, "maxvaluelen", method_logreader_maxvaluelen, 0);

	rb_define_alloc_func(Hash, alloc_hashreader);
	rb_define_singleton_method(Hash, "write", method_hash_write, -2);
	rb_define_method(Hash, "initialize", method_hash_initialize, 2);
	rb_define_method(Hash, "close", method_hash_close, 0);
	rb_define_method(Hash, "each", method_hash_each, 0);
	rb_define_method(Hash, "get", method_hash_get, 1);
	rb_define_method(Hash, "maxkeylen", method_hash_maxkeylen, 0);
	rb_define_method(Hash, "maxvaluelen", method_hash_maxvaluelen, 0);
	rb_define_method(Hash, "length", method_hash_length, 0);
	rb_define_method(Hash, "collisions", method_hash_collisions, 0);
	rb_define_method(Hash, "open?", method_hash_open, 0);

	GnistaException = rb_define_class("GnistaException", rb_eException);
}
