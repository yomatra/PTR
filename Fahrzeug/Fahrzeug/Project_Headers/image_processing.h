#ifndef IMAGE_PROCESSING_H_
#define IMAGE_PROCESSING_H_


#ifdef __cplusplus
extern "C" {
#endif

#define CAMLINE_LENGTH 128			// Anzahl der Pixel einer Kamerazeile (sensorbedingt)
#define MAX_EDGES 13				// Maximal erlaubte Anzahl an erkannten Kanten pro Auswertung + 1 (letzter Wert: Anzahl)
#define LINE_WIDTH_MIN 2			// minimale erwartete Linienbreite in Pixel
#define LINE_WIDTH_MAX 5			// maximale erwartete Linienbreite in Pixel
#define EDGE_THRESHOLD 50			// Schwellwert für Ableitungssignal/Kantenstärke 		

// Funktionen exportieren
__declspec(dllexport) void sobel_filter(uint8_t camline[], int8_t derivative_line[]);
__declspec(dllexport) void edge_locations(int8_t derivative_line[], uint8_t edge_locations[], uint8_t offset);
__declspec(dllexport) uint8_t line_location(uint8_t edge_locations[], uint8_t line_info[]);

#ifdef __cplusplus
}
#endif

#endif // IMAGE_PROCESSING_H_

/* eingestellte Höhe der unteren Kamera (kleine Brennweite): 
 * von Boden bis Unterkante Kamerahalter: 13 cm
 */
