#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

G_MODULE_EXPORT void on_window1_destroy(GtkObject* object, gpointer user_data);
G_MODULE_EXPORT void on_window1_realize(GtkObject* object, gpointer user_data);

G_MODULE_EXPORT int rafraichir_affichage_alaire();
G_MODULE_EXPORT void rafraichir_affichage_pt();

G_MODULE_EXPORT void on_spinbutton_emplanture_value_changed(GtkRadioButton* rdb, gpointer user_data);
G_MODULE_EXPORT void on_spinbutton_extremite_value_changed(GtkRadioButton* rdb, gpointer user_data);
G_MODULE_EXPORT void on_spinbutton_envergure_value_changed(GtkRadioButton* rdb, gpointer user_data);

G_MODULE_EXPORT void on_hscale_altitude_value_changed(GtkHScale* hsc, gpointer user_data);
G_MODULE_EXPORT void on_spinbutton_altitude_value_changed(GtkSpinButton* spb, gpointer user_data);

G_MODULE_EXPORT void on_hscale_vitesse_value_changed(GtkHScale* hsc, gpointer user_data);
G_MODULE_EXPORT void on_spinbutton_vitesse_value_changed(GtkSpinButton* spb, gpointer user_data);

G_MODULE_EXPORT void on_spinbutton_cz_value_changed(GtkSpinButton* spb, gpointer user_data);
G_MODULE_EXPORT void on_spinbutton_cx_value_changed(GtkSpinButton* spb, gpointer user_data);

G_MODULE_EXPORT void on_radiobutton_ept_toggled(GtkRadioButton* rdb, gpointer user_data);
G_MODULE_EXPORT void on_radiobutton_G_rdb_coef_toggled(GtkRadioButton* rdb, gpointer user_data);

G_MODULE_EXPORT void on_button_export_csv_clicked(GtkButton* btn, gpointer user_data);
#endif
