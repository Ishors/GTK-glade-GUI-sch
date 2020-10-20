#include <stdio.h>
#include <stdlib.h>
#include "callbacks.h"
#include <gtk/gtk.h>
#include <windows.h>
#include <math.h>

GtkBuilder* builder;
int G_nb_coef = 0;
GtkRadioButton* G_rdb_coef[100];

void on_window1_destroy(GtkObject* object, gpointer user_data) {
	FILE* out;
	gdouble Emp, Ext, Env, CMA, S, Alt, V, Cx, Cz;
	int i, savoir = 0;

	GtkSpinButton* spinbutton_emplanture = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_emplanture"));
	GtkSpinButton* spinbutton_extremite = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_extremite"));
	GtkSpinButton* spinbutton_envergure = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_envergure"));

	GtkHScale* hscale_altitude = GTK_HSCALE(gtk_builder_get_object(builder, "hscale_altitude"));
	GtkSpinButton* spinbutton_altitude = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_altitude"));

	GtkHScale* hscale_vitesse = GTK_HSCALE(gtk_builder_get_object(builder, "hscale_vitesse"));
	GtkSpinButton* spinbutton_vitesse = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_vitesse"));

	GtkSpinButton* spinbutton_cz = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_cz"));
	GtkSpinButton* spinbutton_cx = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_cx"));

	/* r�cup�ration des values actuelles des spinbuttons et hscale */
	Cz = gtk_spin_button_get_value(spinbutton_cz);
	Cx = gtk_spin_button_get_value(spinbutton_cx);
	Alt = gtk_spin_button_get_value(spinbutton_altitude);
	V = gtk_spin_button_get_value(spinbutton_vitesse);

	/* Calcul de la corde moyenne et de la surface alaire */
	Emp = gtk_spin_button_get_value(spinbutton_emplanture);
	Ext = gtk_spin_button_get_value(spinbutton_extremite);
	Env = gtk_spin_button_get_value(spinbutton_envergure);
	CMA = (2. / 3.) * (((Emp * Emp) + (Emp * Ext) + (Ext * Ext)) / (Emp + Ext));
	S = CMA * Env;

	for (i = 0; i < G_nb_coef; i++) {
		if (gtk_toggle_button_get_active(G_rdb_coef[i])) {
			savoir = i + 1;
		}
	}
	printf("%d", savoir);

	/* �criture des donn�es dans le fichier */
	errno_t err;
	err = fopen_s(&out, "configuration", "w");
	if (err != 0) {
		perror("Erreur ouverture fichier en �criture\n");
	}
	else {
		fwrite(&Emp, sizeof(gdouble), 1, out);
		fwrite(&Ext, sizeof(gdouble), 1, out);
		fwrite(&Env, sizeof(gdouble), 1, out);
		fwrite(&CMA, sizeof(gdouble), 1, out);
		fwrite(&S, sizeof(gdouble), 1, out);
		fwrite(&Alt, sizeof(gdouble), 1, out);
		fwrite(&V, sizeof(gdouble), 1, out);
		fwrite(&Cz, sizeof(gdouble), 1, out);
		fwrite(&Cx, sizeof(gdouble), 1, out);
		fwrite(&savoir, sizeof(int), 1, out);
		fclose(out);
	}

	printf("Fermeture de la fenetre.\n");
	gtk_main_quit();

}

/* fonctions d'initialisation des spinbuttons */
void init_spin_button(GtkSpinButton* spb, double min_val, double max_val, double val, double step, int digits) {
	gtk_spin_button_set_digits(spb, digits);
	gtk_spin_button_set_range(spb, min_val, max_val);
	gtk_spin_button_set_value(spb, val);
	gtk_spin_button_set_increments(spb, step, step);
}

/* fonctions d'initialisation des hscales */
void init_hscale(GtkHScale* hsc, double min_val, double max_val, double val, double step, int digits) {
	gtk_scale_set_digits(GTK_SCALE(hsc), digits);
	gtk_range_set_range(GTK_RANGE(hsc), min_val, max_val);
	gtk_range_set_increments(GTK_RANGE(hsc), step, step);
	gtk_range_set_value(GTK_RANGE(hsc), val);
}

void on_radiobutton_G_rdb_coef_toggled(GtkRadioButton* rdb, gpointer user_data) {
	rafraichir_affichage_pt();
}

void on_radiobutton_ept_toggled(GtkRadioButton* rdb, gpointer user_data) {
	rafraichir_affichage_pt();
}

/* initialisation des spinbuttons et hscales */
void on_window1_realize(GtkObject* object, gpointer user_data) {
	FILE* in;
	FILE* coef;
	gdouble Emp, Ext, Env, CMA, S, Alt, V, Cz, Cx;
	float b = 0, c = 0;
	int savoir;
	int i = 0;
	char a[128], g[128], freestyle[128];
	G_nb_coef = 0;


	GtkSpinButton* spinbutton_emplanture = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_emplanture"));
	GtkSpinButton* spinbutton_extremite = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_extremite"));
	GtkSpinButton* spinbutton_envergure = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_envergure"));

	GtkHScale* hscale_altitude = GTK_HSCALE(gtk_builder_get_object(builder, "hscale_altitude"));
	GtkSpinButton* spinbutton_altitude = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_altitude"));

	GtkHScale* hscale_vitesse = GTK_HSCALE(gtk_builder_get_object(builder, "hscale_vitesse"));
	GtkSpinButton* spinbutton_vitesse = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_vitesse"));

	GtkSpinButton* spinbutton_cz = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_cz"));
	GtkSpinButton* spinbutton_cx = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_cx"));

	/* Lecture du fichier binaire contenant les donn�es enregistr�es  en quittant la fen�tre*/
	errno_t err;
	err = fopen_s(&in, "configuration", "r");

	if (err != 0) {
		perror("Erreur ouverture fichier en lecture\n");
	}
	else {
		fread(&Emp, sizeof(gdouble), 1, in);
		fread(&Ext, sizeof(gdouble), 1, in);
		fread(&Env, sizeof(gdouble), 1, in);
		fread(&CMA, sizeof(gdouble), 1, in);
		fread(&S, sizeof(gdouble), 1, in);
		fread(&Alt, sizeof(gdouble), 1, in);
		fread(&V, sizeof(gdouble), 1, in);
		fread(&Cz, sizeof(gdouble), 1, in);
		fread(&Cx, sizeof(gdouble), 1, in);
		fread(&savoir, sizeof(int), 1, in);
		printf("Emp=%.2lf Ext=%.2lf Env=%.2lf CMA=%.2lf S=%lf Alt= %.2lf V=%.2lf Cz=%.2lf Cx=%lf savoir=%d\n", Emp, Ext, Env, CMA, S, Alt, V, Cz, Cx, savoir);
		fclose(in);
	}

	/* Lecture du fichier ASCII */
	GtkRadioButton* radiobutton_ept = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "radiobutton_ept"));
	GtkVBox* vbox_predefini = GTK_VBOX(gtk_builder_get_object(builder, "vbox_predefini"));

	errno_t err2;
	err2 = fopen_s(&coef, "coefficients", "r");

	if (err2 != 0) {
		perror("Erreur ouverture fichier en lecture\n");
	}
	while (!feof(coef)) {
		fgets(a, 128, coef);
		fscanf_s(coef, "%f", &b);
		fscanf_s(coef, "%f", &c);
		fgets(g, 128, coef);
		sprintf_s(freestyle, 128, "%s (Cz=%.2f , Cx=%.3f)", a, b, c);
		G_rdb_coef[i] = gtk_radio_button_new_with_label_from_widget(radiobutton_ept, freestyle);
		g_signal_connect(G_rdb_coef[i], "toggled", G_CALLBACK(on_radiobutton_G_rdb_coef_toggled), NULL);
		gtk_box_pack_start(vbox_predefini, G_rdb_coef[i], TRUE, TRUE, 0);
		gtk_widget_show(G_rdb_coef[i]);
		i++;
		G_nb_coef++;
		//printf("%d", &G_nb_coef);
	}
	fclose(coef);
	
	if (savoir != 0) {
		gtk_toggle_button_set_active(G_rdb_coef[savoir - 1], TRUE);
		gtk_toggle_button_set_active(radiobutton_ept, FALSE);
		
	}
	else {
		gtk_toggle_button_set_active(radiobutton_ept, TRUE);
		
	
	}

	init_spin_button(spinbutton_emplanture, 0.0, 100, Emp, 1, 1);

	init_spin_button(spinbutton_extremite, 0.0, 100, Ext, 1, 1);
	init_spin_button(spinbutton_envergure, 0.0, 100, Env, 1, 1);

	init_hscale(hscale_altitude, 1000, 40000, Alt, 10, 1);
	init_spin_button(spinbutton_altitude, 1000, 40000, Alt, 10, 1);

	init_hscale(hscale_vitesse, 100, 3500, V, 2, 1);
	init_spin_button(spinbutton_vitesse, 100, 3500, V, 2, 1);

	init_spin_button(spinbutton_cz, 0.0, 5, Cz, 0.1, 1);
	init_spin_button(spinbutton_cx, 0.0, 0.03, Cx, 0.002, 3);
}

int rafraichir_affichage_alaire() {
	double Emp, Ext, Env, CMA, S;
	char msg1[128], msg2[128];
	GtkSpinButton* spinbutton_emplanture = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_emplanture"));
	GtkSpinButton* spinbutton_extremite = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_extremite"));
	GtkSpinButton* spinbutton_envergure = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_envergure"));

	/* Calcul de la corde moyenne de l'aile */
	Emp = gtk_spin_button_get_value(spinbutton_emplanture);
	Ext = gtk_spin_button_get_value(spinbutton_extremite);
	Env = gtk_spin_button_get_value(spinbutton_envergure);

	CMA = (2./3.)*(((Emp * Emp) + (Emp * Ext) + (Ext * Ext)) / (Emp + Ext));

	/* Calcul de la surface alaire de l'avion */
	S = CMA * Env;

	/* Affichage des r�sultats dans les labels */
	GtkLabel* label_cmoy = GTK_LABEL(gtk_builder_get_object(builder, "label_cmoy"));
	sprintf_s(msg1, 128, "Corde moyenne de l'aile : %.2lf m", CMA);
	gtk_label_set_text(label_cmoy, msg1);

	GtkLabel* label_salaire = GTK_LABEL(gtk_builder_get_object(builder, "label_salaire"));
	sprintf_s(msg2, 128, "Surface alaire de l'avion : %.2lf m2", S);
	gtk_label_set_text(label_salaire, msg2);

	return 1;
}

void rafraichir_affichage_pt() {
	double Emp, Ext, Env, CMA, S, Alt, V, Vs, Cz, Cx, rho, P, T, F;
	gchar *msg1 = "";
	gchar *msg2 = "";
	gchar *msg3 = "";
	FILE* coef;
	char a[128], g[128];
	double b = 0, c = 0;
	int i, j = 0;

	GtkSpinButton* spinbutton_emplanture = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_emplanture"));
	GtkSpinButton* spinbutton_extremite = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_extremite"));
	GtkSpinButton* spinbutton_envergure = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_envergure"));

	GtkHScale* hscale_altitude = GTK_HSCALE(gtk_builder_get_object(builder, "hscale_altitude"));
	GtkSpinButton* spinbutton_altitude = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_altitude"));

	GtkHScale* hscale_vitesse = GTK_HSCALE(gtk_builder_get_object(builder, "hscale_vitesse"));
	GtkSpinButton* spinbutton_vitesse = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_vitesse"));

	GtkSpinButton* spinbutton_cz = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_cz"));
	GtkSpinButton* spinbutton_cx = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_cx"));

	GtkLabel* label_portance = GTK_LABEL(gtk_builder_get_object(builder, "label_portance"));
	GtkLabel* label_trainee = GTK_LABEL(gtk_builder_get_object(builder, "label_trainee"));
	GtkLabel* label_finesse = GTK_LABEL(gtk_builder_get_object(builder, "label_finesse"));
	GtkLabel* label_altitude = GTK_LABEL(gtk_builder_get_object(builder, "label_altitude"));
	GtkLabel* label_vitesse = GTK_LABEL(gtk_builder_get_object(builder, "label_vitesse"));
	GtkLabel* label_cz = GTK_LABEL(gtk_builder_get_object(builder, "label_cz"));
	GtkLabel* label_cx = GTK_LABEL(gtk_builder_get_object(builder, "label_cx"));

	/* Calcul de la corde moyenne de l'aile */
	Emp = gtk_spin_button_get_value(spinbutton_emplanture);
	Ext = gtk_spin_button_get_value(spinbutton_extremite);
	Env = gtk_spin_button_get_value(spinbutton_envergure);
	V = gtk_spin_button_get_value(spinbutton_vitesse);
	Alt = gtk_spin_button_get_value(spinbutton_altitude);

	CMA = ((2. / 3.) * ((Emp * Emp) + (Emp * Ext) + (Ext * Ext)) / (Emp + Ext));

	/* Calcul de la surface alaire de l'avion */
	S = CMA * Env;
	GtkRadioButton* radiobutton_ept = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "radiobutton_ept"));

	if (rafraichir_affichage_alaire() && S > 0. && S <= 10000.) {

	

		for (i = 0; i < G_nb_coef; i++) {
			if (gtk_toggle_button_get_active(radiobutton_ept)) {
				Cz = gtk_spin_button_get_value(spinbutton_cz);
				Cx = gtk_spin_button_get_value(spinbutton_cx);
				gtk_widget_set_sensitive(label_cz, TRUE);
				gtk_widget_set_sensitive(spinbutton_cz, TRUE);
				gtk_widget_set_sensitive(label_cx, TRUE);
				gtk_widget_set_sensitive(spinbutton_cx, TRUE);
			}
			else if (gtk_toggle_button_get_active(G_rdb_coef[i])) {
				errno_t err2;
				err2 = fopen_s(&coef, "coefficients", "r");

				if (err2 != 0) {
					perror("Erreur ouverture fichier en lecture\n");
				}
				while (j < i + 1) {
					fgets(a, 128, coef);
					fscanf_s(coef, "%lf", &b);
					printf("%f", b);
					fscanf_s(coef, "%lf", &c);
					printf("%f", c);
					fgets(g, 128, coef);
					j++;
				}
				fclose(coef);
				Cz = b;
				Cx = c;
				gtk_widget_set_sensitive(label_cz, FALSE);
				gtk_widget_set_sensitive(spinbutton_cz, FALSE);
				gtk_widget_set_sensitive(label_cx, FALSE);
				gtk_widget_set_sensitive(spinbutton_cx, FALSE);
			}
		}

		
		/* rho � modifie en fction de l'altitude */
		rho = 1.292;
		rho /= ((((int)(Alt * 0.3048)) % 5000) + 1);

		/* Conversion km/h en m/s */
		Vs = V / 3.6;

		/* Calcul des forces en MN et de fa finesse */
		P = (0.5 * rho * Vs * Vs * S * Cz) / 1000;
		T = (0.5 * rho * Vs * Vs * S * Cx) / 1000;
		F = P / T;


		/*GtkLabel* label_portance = GTK_LABEL(gtk_builder_get_object(builder, "label_portance"));*/

		msg1 = g_markup_printf_escaped("<b><span foreground=\"#088815\">Force de portance de : %.2lf kN   </span></b>", P);
		gtk_label_set_markup(GTK_LABEL(label_portance), msg1);
		g_free(msg1);

		/*GtkLabel* label_trainee = GTK_LABEL(gtk_builder_get_object(builder, "label_trainee"));*/

		msg2 = g_markup_printf_escaped("<b><span foreground=\"#F91C1C\">Force de trainee de : %.2lf kN   </span></b>", T);
		gtk_label_set_markup(GTK_LABEL(label_trainee), msg2);
		g_free(msg2);

		/*GtkLabel* label_finesse = GTK_LABEL(gtk_builder_get_object(builder, "label_finesse"));*/

		if (gtk_toggle_button_get_active(radiobutton_ept))
			msg3 = g_markup_printf_escaped("<b><span foreground=\"#324D8F\">Finesse de : %.2lf    </span></b>", F);
		else
			msg3 = g_markup_printf_escaped("<b><span foreground=\"#8C7F02\">Finesse de : %.2lf    </span></b>", F);

		gtk_label_set_markup(GTK_LABEL(label_finesse), msg3);
		g_free(msg3);
		

		gtk_widget_set_sensitive(hscale_altitude, TRUE);
		gtk_widget_set_sensitive(spinbutton_altitude, TRUE);
		gtk_widget_set_sensitive(hscale_vitesse, TRUE);
		gtk_widget_set_sensitive(spinbutton_vitesse, TRUE);
		gtk_widget_set_sensitive(label_portance, TRUE);
		gtk_widget_set_sensitive(label_trainee, TRUE);
		gtk_widget_set_sensitive(label_finesse, TRUE);
		gtk_widget_set_sensitive(label_altitude, TRUE);
		gtk_widget_set_sensitive(label_vitesse, TRUE);
		gtk_widget_set_sensitive(radiobutton_ept, TRUE);
		for (i = 0; i < G_nb_coef; i++) {
			gtk_widget_set_sensitive(G_rdb_coef[i], TRUE);
		}
		
		
	}
	else {

		gtk_widget_set_sensitive(hscale_altitude, FALSE);
		gtk_widget_set_sensitive(spinbutton_altitude, FALSE);
		gtk_widget_set_sensitive(hscale_vitesse, FALSE);
		gtk_widget_set_sensitive(spinbutton_vitesse, FALSE);
		gtk_widget_set_sensitive(spinbutton_cz, FALSE);
		gtk_widget_set_sensitive(spinbutton_cx, FALSE);
		gtk_widget_set_sensitive(label_cx, FALSE);
		gtk_widget_set_sensitive(label_cz, FALSE);
		gtk_widget_set_sensitive(label_portance, FALSE);
		gtk_widget_set_sensitive(label_trainee, FALSE);
		gtk_widget_set_sensitive(label_finesse, FALSE);
		gtk_widget_set_sensitive(label_altitude, FALSE);
		gtk_widget_set_sensitive(label_vitesse, FALSE);
		gtk_widget_set_sensitive(radiobutton_ept, FALSE);
		for (i = 0; i < G_nb_coef; i++) {
			gtk_widget_set_sensitive(G_rdb_coef[i], FALSE);
		}
		
		/*GtkLabel* label_portance = GTK_LABEL(gtk_builder_get_object(builder, "label_portance"));*/

		msg1 = g_markup_printf_escaped("<b><span foreground=\"#088815\">Force de portance de : (en kN)   </span></b>");
		gtk_label_set_markup(GTK_LABEL(label_portance), msg1);
		g_free(msg1);

		/*GtkLabel* label_trainee = GTK_LABEL(gtk_builder_get_object(builder, "label_trainee"));*/
		
		msg2 = g_markup_printf_escaped("<b><span foreground=\"#F91C1C\">Force de trainee de : (en kN)   </span></b>");
		gtk_label_set_markup(GTK_LABEL(label_trainee), msg2);
		g_free(msg2);

		/*GtkLabel* label_finesse = GTK_LABEL(gtk_builder_get_object(builder, "label_finesse"));*/

		if (gtk_toggle_button_get_active(radiobutton_ept))
			msg3 = g_markup_printf_escaped("<b><span foreground=\"#324D8F\">Finesse de : (sans unite)    </span></b>");
		else
			msg3 = g_markup_printf_escaped("<b><span foreground=\"#8C7F02\">Finesse de : (sans unite)    </span></b>");

		gtk_label_set_markup(GTK_LABEL(label_finesse), msg3);
		g_free(msg3);
	}	

}

void on_spinbutton_emplanture_value_changed(GtkRadioButton* rdb, gpointer user_data) {
	
	rafraichir_affichage_pt();
}

void on_spinbutton_extremite_value_changed(GtkRadioButton* rdb, gpointer user_data) {
	
	rafraichir_affichage_pt();
}

void on_spinbutton_envergure_value_changed(GtkRadioButton* rdb, gpointer user_data) {
	
	rafraichir_affichage_pt();
}

/* modification spinbutton et hscale altitude */
void on_hscale_altitude_value_changed(GtkHScale* hsc, gpointer user_data) {
	GtkSpinButton* spb = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_altitude"));
	gdouble val = gtk_range_get_value(hsc);
	gtk_spin_button_set_value(spb, val);
}

void on_spinbutton_altitude_value_changed(GtkSpinButton* spb, gpointer user_data) {
	GtkHScale* hsc = GTK_RANGE(gtk_builder_get_object(builder, "hscale_altitude"));
	gdouble val = gtk_spin_button_get_value(spb);
	gtk_range_set_value(GTK_RANGE(hsc), val);

	rafraichir_affichage_pt();
}

/* modification spinbutton et hscale vitesse */
void on_hscale_vitesse_value_changed(GtkHScale* hsc, gpointer user_data) {
	GtkSpinButton* spb = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_vitesse"));
	gdouble val = gtk_range_get_value(GTK_RANGE(hsc));
	gtk_spin_button_set_value(spb, val);
}

void on_spinbutton_vitesse_value_changed(GtkSpinButton* spb, gpointer user_data) {
	GtkHScale* hsc = GTK_RANGE(gtk_builder_get_object(builder, "hscale_vitesse"));
	gdouble val = gtk_spin_button_get_value(spb);
	gtk_range_set_value(GTK_RANGE(hsc), val);

	rafraichir_affichage_pt();
}

void on_spinbutton_cz_value_changed(GtkSpinButton* spb, gpointer user_data) {
	rafraichir_affichage_pt();
}

void on_spinbutton_cx_value_changed(GtkSpinButton* spb, gpointer user_data) {
	rafraichir_affichage_pt();
}

void on_button_export_csv_clicked(GtkButton* btn, gpointer user_data) {
	/* initialisation des constantes n�cessaires au calcul */
	double Emp, Ext, Env, CMA, S, Alt, V, Vs, Cz, Cx, rho, P, T, F;
	int i, j = 0;
	double b = 0, c = 0;
	char a[128], g[128];
	FILE* coef;

	GtkSpinButton* spinbutton_emplanture = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_emplanture"));
	GtkSpinButton* spinbutton_extremite = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_extremite"));
	GtkSpinButton* spinbutton_envergure = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_envergure"));

	GtkHScale* hscale_altitude = GTK_HSCALE(gtk_builder_get_object(builder, "hscale_altitude"));
	GtkSpinButton* spinbutton_altitude = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_altitude"));

	GtkHScale* hscale_vitesse = GTK_HSCALE(gtk_builder_get_object(builder, "hscale_vitesse"));
	GtkSpinButton* spinbutton_vitesse = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_vitesse"));

	GtkSpinButton* spinbutton_cz = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_cz"));
	GtkSpinButton* spinbutton_cx = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "spinbutton_cx"));

	GtkRadioButton* radiobutton_ept = GTK_RADIO_BUTTON(gtk_builder_get_object(builder, "radiobutton_ept"));

	/* Calcul de la corde moyenne de l'aile */
	Emp = gtk_spin_button_get_value(spinbutton_emplanture);
	Ext = gtk_spin_button_get_value(spinbutton_extremite);
	Env = gtk_spin_button_get_value(spinbutton_envergure);
	V = gtk_spin_button_get_value(spinbutton_vitesse);
	Alt = gtk_spin_button_get_value(spinbutton_altitude);

	if (gtk_toggle_button_get_active(radiobutton_ept)) {
		Cz = gtk_spin_button_get_value(spinbutton_cz);
		Cx = gtk_spin_button_get_value(spinbutton_cx);
	}
	else {
		for (i = 0; i < G_nb_coef; i++) {
			if (gtk_toggle_button_get_active(G_rdb_coef[i])) {
				errno_t err2;
				err2 = fopen_s(&coef, "coefficients", "r");

				if (err2 != 0) {
					perror("Erreur ouverture fichier en lecture\n");
				}
				while (j < i + 1) {
					fgets(a, 128, coef);
					fscanf_s(coef, "%lf", &b);
					printf("%f", b);
					fscanf_s(coef, "%lf", &c);
					printf("%f", c);
					fgets(g, 128, coef);
					j++;
				}
				Cz = b;
				Cx = c;
			}
			
		}
	}

	

	CMA = ((2. / 3.) * ((Emp * Emp) + (Emp * Ext) + (Ext * Ext)) / (Emp + Ext));

	/* Calcul de la surface alaire de l'avion */
	S = CMA * Env;

	/* rho � modifie en fction de l'altitude */
	rho = 1.292;
	rho /= ((((int)(Alt * 0.3048)) % 5000) + 1);

	/* Conversion km/h en m/s */
	Vs = V / 3.6;

	/* Calcul des forces en MN et de fa finesse */
	P = (0.5 * rho * Vs * Vs * S * Cz) / 1000;
	T = (0.5 * rho * Vs * Vs * S * Cx) / 1000;
	F = P / T;

	/* r�cup�ration du fichier cible choisit par l'utilisateur */
	gchar* file_name = NULL;
	GtkFileChooser* chs = GTK_FILE_CHOOSER(gtk_builder_get_object(builder, "filechooserbutton_csv"));
	file_name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chs));

	/* �criture des r�sultats dans le fichier */
	printf("=== DEBUT ecriture du fichier %s ===\n", file_name);

	FILE* desc;
	errno_t err;
	err = fopen_s(&desc, file_name, "w");
	if (err != 0) {
		perror("Erreur ouverture fichier en ecriture");
	}
	else {
		fprintf_s(desc, "CORDE EMPLANTURE (m) ; %lf\n", Emp);
		fprintf_s(desc, "CORDE EXTR�MIT� (m) ; %lf\n", Ext);
		fprintf_s(desc, "ENVERGURE(m) ; %lf\n", Env);
		fprintf_s(desc, "CORDE MOYENNE(m) ; %lf\n", CMA);
		fprintf_s(desc, "SURFACE ALAIRE (m2) ; %lf\n", S);
		fprintf_s(desc, "ALTITUDE (ft) ;  %lf\n", Alt);
		fprintf_s(desc, "MASSE VOLUMIQUE AIR (kg/m3) ;  %lf\n", rho);
		fprintf_s(desc, "VITESSE (km/h) ; %lf\n", V);
		fprintf_s(desc, "Cz ; %lf\n", Cz);
		fprintf_s(desc, "Cx ; %lf\n", Cx);
		fprintf_s(desc, "PORTANCE (kN); %lf\n", P);
		fprintf_s(desc, "TRAINEE (kN); %lf\n", T);
		fprintf_s(desc, "FINESSE ; %lf\n", F);
		fclose(desc);
	}

	printf("=== FIN ecriture du fichier %s ===\n", file_name);

	g_free(file_name), file_name = NULL;
}

