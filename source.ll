
voti <-
[
 //Voto   | CFU | Lode
  [ 30,       6,  0], // info teo
  [ 28,       6,  0], // bista
  [ 30,       9,  1], // os
  [ 30,       9,  1], // poggioni
  [ 30,       6,  1], // ottimizzazione
  [ 30,       6,  1], // ughi
  [ 30,      12,  1], // ia
  [ 30,       6,  1], // ssd (9 CFU?)
//[ 30,       6,  1], // tasso (9 CFU?)
//[ 30,       6,  0], // forenze
//[ 30,       6,  0], // molecolari
//[ 30,       6,  0], // parallela
//[(20+27)/2,12,  0]  // nv + pino
]

somma = 0
crediti = 0
lodi_valore = 0

for materia of voti
{
    somma = somma + materia[0] * materia[1]
    crediti = crediti + materia[1]
    
    if materia[2] > 0
    {
        if materia[1] == 6  { lodi_valore = lodi_valore+0.25 }
        if materia[1] == 9  { lodi_valore = lodi_valore+0.35 }
        if materia[1] == 12 { lodi_valore = lodi_valore+0.5  }
    }
}
//limite valore lodi
if lodi_valore > 2.0 {  lodi_valore = 2.0 }

voto = (somma / crediti) / 30 * 110
voto_con_lodi = voto + lodi_valore

print("crediti:\t",crediti, "\n")
print("media ponderata:\t", voto, "\n")
print("valore delle lodi:\t", lodi_valore, "\n")
print("voto con cui ci si presenta:\t", voto_con_lodi, "\n")
