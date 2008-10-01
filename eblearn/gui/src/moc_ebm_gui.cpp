/****************************************************************************
** Meta object code from reading C++ file 'ebm_gui.h'
**
** Created: Mon Sep 8 15:05:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/ebm_gui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ebm_gui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_ebbox[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       7,    6,    6,    6, 0x0a,
      24,    6,    6,    6, 0x0a,
      33,    6,    6,    6, 0x0a,
      41,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ebbox[] = {
    "ebbox\0\0openProperties()\0resize()\0"
    "print()\0refreshdisplay()\0"
};

const QMetaObject ebbox::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_ebbox,
      qt_meta_data_ebbox, 0 }
};

const QMetaObject *ebbox::metaObject() const
{
    return &staticMetaObject;
}

void *ebbox::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ebbox))
	return static_cast<void*>(const_cast< ebbox*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int ebbox::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: openProperties(); break;
        case 1: resize(); break;
        case 2: print(); break;
        case 3: refreshform(); break;
        }
        _id -= 4;
    }
    return _id;
}
static const uint qt_meta_data_idx_propgui[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      32,   13,   12,   12, 0x0a,
      71,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_idx_propgui[] = {
    "idx_propgui\0\0plotdata,printdata\0"
    "get_data(idx_plotdata*,idx_printdata*)\0"
    "choose_file()\0"
};

const QMetaObject idx_propgui::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_idx_propgui,
      qt_meta_data_idx_propgui, 0 }
};

const QMetaObject *idx_propgui::metaObject() const
{
    return &staticMetaObject;
}

void *idx_propgui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_idx_propgui))
	return static_cast<void*>(const_cast< idx_propgui*>(this));
    return QDialog::qt_metacast(_clname);
}

int idx_propgui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: get_data((*reinterpret_cast< idx_plotdata*(*)>(_a[1])),(*reinterpret_cast< idx_printdata*(*)>(_a[2]))); break;
        case 1: choose_file(); break;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_idx_displayGui[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_idx_displayGui[] = {
    "idx_displayGui\0"
};

const QMetaObject idx_displayGui::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_idx_displayGui,
      qt_meta_data_idx_displayGui, 0 }
};

const QMetaObject *idx_displayGui::metaObject() const
{
    return &staticMetaObject;
}

void *idx_displayGui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_idx_displayGui))
	return static_cast<void*>(const_cast< idx_displayGui*>(this));
    return QWidget::qt_metacast(_clname);
}

int idx_displayGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_Idx_Gui[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x0a,
      26,    8,    8,    8, 0x0a,
      40,   34,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Idx_Gui[] = {
    "Idx_Gui\0\0refreshdisplay()\0print()\0"
    "event\0keyPressEvent(QKeyEvent*)\0"
};

const QMetaObject Idx_Gui::staticMetaObject = {
    { &ebbox::staticMetaObject, qt_meta_stringdata_Idx_Gui,
      qt_meta_data_Idx_Gui, 0 }
};

const QMetaObject *Idx_Gui::metaObject() const
{
    return &staticMetaObject;
}

void *Idx_Gui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Idx_Gui))
	return static_cast<void*>(const_cast< Idx_Gui*>(this));
    return ebbox::qt_metacast(_clname);
}

int Idx_Gui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ebbox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refreshdisplay(); break;
        case 1: print(); break;
        case 2: keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_state_idx_propgui[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   19,   18,   18, 0x0a,
      50,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_state_idx_propgui[] = {
    "state_idx_propgui\0\0data\0"
    "get_data(state_idx_data*)\0choose_file()\0"
};

const QMetaObject state_idx_propgui::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_state_idx_propgui,
      qt_meta_data_state_idx_propgui, 0 }
};

const QMetaObject *state_idx_propgui::metaObject() const
{
    return &staticMetaObject;
}

void *state_idx_propgui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_state_idx_propgui))
	return static_cast<void*>(const_cast< state_idx_propgui*>(this));
    return QDialog::qt_metacast(_clname);
}

int state_idx_propgui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: get_data((*reinterpret_cast< state_idx_data*(*)>(_a[1]))); break;
        case 1: choose_file(); break;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_state_idx_displayGui[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_state_idx_displayGui[] = {
    "state_idx_displayGui\0"
};

const QMetaObject state_idx_displayGui::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_state_idx_displayGui,
      qt_meta_data_state_idx_displayGui, 0 }
};

const QMetaObject *state_idx_displayGui::metaObject() const
{
    return &staticMetaObject;
}

void *state_idx_displayGui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_state_idx_displayGui))
	return static_cast<void*>(const_cast< state_idx_displayGui*>(this));
    return QWidget::qt_metacast(_clname);
}

int state_idx_displayGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_state_Idx_Gui[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      32,   14,   14,   14, 0x0a,
      46,   40,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_state_Idx_Gui[] = {
    "state_Idx_Gui\0\0refreshdisplay()\0print()\0"
    "event\0keyPressEvent(QKeyEvent*)\0"
};

const QMetaObject state_Idx_Gui::staticMetaObject = {
    { &ebbox::staticMetaObject, qt_meta_stringdata_state_Idx_Gui,
      qt_meta_data_state_Idx_Gui, 0 }
};

const QMetaObject *state_Idx_Gui::metaObject() const
{
    return &staticMetaObject;
}

void *state_Idx_Gui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_state_Idx_Gui))
	return static_cast<void*>(const_cast< state_Idx_Gui*>(this));
    return ebbox::qt_metacast(_clname);
}

int state_Idx_Gui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ebbox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refreshdisplay(); break;
        case 1: print(); break;
        case 2: keyPressEvent((*reinterpret_cast< QKeyEvent*(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_module_1_1_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_module_1_1_GUI[] = {
    "module_1_1_GUI\0"
};

const QMetaObject module_1_1_GUI::staticMetaObject = {
    { &ebbox::staticMetaObject, qt_meta_stringdata_module_1_1_GUI,
      qt_meta_data_module_1_1_GUI, 0 }
};

const QMetaObject *module_1_1_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *module_1_1_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_module_1_1_GUI))
	return static_cast<void*>(const_cast< module_1_1_GUI*>(this));
    return ebbox::qt_metacast(_clname);
}

int module_1_1_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ebbox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_module_2_1_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_module_2_1_GUI[] = {
    "module_2_1_GUI\0"
};

const QMetaObject module_2_1_GUI::staticMetaObject = {
    { &ebbox::staticMetaObject, qt_meta_stringdata_module_2_1_GUI,
      qt_meta_data_module_2_1_GUI, 0 }
};

const QMetaObject *module_2_1_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *module_2_1_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_module_2_1_GUI))
	return static_cast<void*>(const_cast< module_2_1_GUI*>(this));
    return ebbox::qt_metacast(_clname);
}

int module_2_1_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ebbox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_layers_2_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_layers_2_GUI[] = {
    "layers_2_GUI\0"
};

const QMetaObject layers_2_GUI::staticMetaObject = {
    { &ebbox::staticMetaObject, qt_meta_stringdata_layers_2_GUI,
      qt_meta_data_layers_2_GUI, 0 }
};

const QMetaObject *layers_2_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *layers_2_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_layers_2_GUI))
	return static_cast<void*>(const_cast< layers_2_GUI*>(this));
    return ebbox::qt_metacast(_clname);
}

int layers_2_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ebbox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_fc_ebm1_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_fc_ebm1_GUI[] = {
    "fc_ebm1_GUI\0"
};

const QMetaObject fc_ebm1_GUI::staticMetaObject = {
    { &ebbox::staticMetaObject, qt_meta_stringdata_fc_ebm1_GUI,
      qt_meta_data_fc_ebm1_GUI, 0 }
};

const QMetaObject *fc_ebm1_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *fc_ebm1_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_fc_ebm1_GUI))
	return static_cast<void*>(const_cast< fc_ebm1_GUI*>(this));
    return ebbox::qt_metacast(_clname);
}

int fc_ebm1_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ebbox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_fc_ebm2_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_fc_ebm2_GUI[] = {
    "fc_ebm2_GUI\0"
};

const QMetaObject fc_ebm2_GUI::staticMetaObject = {
    { &ebbox::staticMetaObject, qt_meta_stringdata_fc_ebm2_GUI,
      qt_meta_data_fc_ebm2_GUI, 0 }
};

const QMetaObject *fc_ebm2_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *fc_ebm2_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_fc_ebm2_GUI))
	return static_cast<void*>(const_cast< fc_ebm2_GUI*>(this));
    return ebbox::qt_metacast(_clname);
}

int fc_ebm2_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ebbox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_linear_module_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_linear_module_GUI[] = {
    "linear_module_GUI\0"
};

const QMetaObject linear_module_GUI::staticMetaObject = {
    { &module_1_1_GUI::staticMetaObject, qt_meta_stringdata_linear_module_GUI,
      qt_meta_data_linear_module_GUI, 0 }
};

const QMetaObject *linear_module_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *linear_module_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_linear_module_GUI))
	return static_cast<void*>(const_cast< linear_module_GUI*>(this));
    return module_1_1_GUI::qt_metacast(_clname);
}

int linear_module_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = module_1_1_GUI::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_nn_layer_full_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_nn_layer_full_GUI[] = {
    "nn_layer_full_GUI\0"
};

const QMetaObject nn_layer_full_GUI::staticMetaObject = {
    { &module_1_1_GUI::staticMetaObject, qt_meta_stringdata_nn_layer_full_GUI,
      qt_meta_data_nn_layer_full_GUI, 0 }
};

const QMetaObject *nn_layer_full_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *nn_layer_full_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_nn_layer_full_GUI))
	return static_cast<void*>(const_cast< nn_layer_full_GUI*>(this));
    return module_1_1_GUI::qt_metacast(_clname);
}

int nn_layer_full_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = module_1_1_GUI::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_f_layer_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_f_layer_GUI[] = {
    "f_layer_GUI\0"
};

const QMetaObject f_layer_GUI::staticMetaObject = {
    { &module_1_1_GUI::staticMetaObject, qt_meta_stringdata_f_layer_GUI,
      qt_meta_data_f_layer_GUI, 0 }
};

const QMetaObject *f_layer_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *f_layer_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_f_layer_GUI))
	return static_cast<void*>(const_cast< f_layer_GUI*>(this));
    return module_1_1_GUI::qt_metacast(_clname);
}

int f_layer_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = module_1_1_GUI::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_c_layer_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_c_layer_GUI[] = {
    "c_layer_GUI\0"
};

const QMetaObject c_layer_GUI::staticMetaObject = {
    { &module_1_1_GUI::staticMetaObject, qt_meta_stringdata_c_layer_GUI,
      qt_meta_data_c_layer_GUI, 0 }
};

const QMetaObject *c_layer_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *c_layer_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_c_layer_GUI))
	return static_cast<void*>(const_cast< c_layer_GUI*>(this));
    return module_1_1_GUI::qt_metacast(_clname);
}

int c_layer_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = module_1_1_GUI::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_s_layer_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_s_layer_GUI[] = {
    "s_layer_GUI\0"
};

const QMetaObject s_layer_GUI::staticMetaObject = {
    { &module_1_1_GUI::staticMetaObject, qt_meta_stringdata_s_layer_GUI,
      qt_meta_data_s_layer_GUI, 0 }
};

const QMetaObject *s_layer_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *s_layer_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_s_layer_GUI))
	return static_cast<void*>(const_cast< s_layer_GUI*>(this));
    return module_1_1_GUI::qt_metacast(_clname);
}

int s_layer_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = module_1_1_GUI::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_logadd_layer_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_logadd_layer_GUI[] = {
    "logadd_layer_GUI\0"
};

const QMetaObject logadd_layer_GUI::staticMetaObject = {
    { &module_1_1_GUI::staticMetaObject, qt_meta_stringdata_logadd_layer_GUI,
      qt_meta_data_logadd_layer_GUI, 0 }
};

const QMetaObject *logadd_layer_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *logadd_layer_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_logadd_layer_GUI))
	return static_cast<void*>(const_cast< logadd_layer_GUI*>(this));
    return module_1_1_GUI::qt_metacast(_clname);
}

int logadd_layer_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = module_1_1_GUI::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_edist_cost_GUI[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_edist_cost_GUI[] = {
    "edist_cost_GUI\0"
};

const QMetaObject edist_cost_GUI::staticMetaObject = {
    { &module_1_1_GUI::staticMetaObject, qt_meta_stringdata_edist_cost_GUI,
      qt_meta_data_edist_cost_GUI, 0 }
};

const QMetaObject *edist_cost_GUI::metaObject() const
{
    return &staticMetaObject;
}

void *edist_cost_GUI::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_edist_cost_GUI))
	return static_cast<void*>(const_cast< edist_cost_GUI*>(this));
    return module_1_1_GUI::qt_metacast(_clname);
}

int edist_cost_GUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = module_1_1_GUI::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ebwindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_ebwindow[] = {
    "ebwindow\0"
};

const QMetaObject ebwindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ebwindow,
      qt_meta_data_ebwindow, 0 }
};

const QMetaObject *ebwindow::metaObject() const
{
    return &staticMetaObject;
}

void *ebwindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ebwindow))
	return static_cast<void*>(const_cast< ebwindow*>(this));
    return QWidget::qt_metacast(_clname);
}

int ebwindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
