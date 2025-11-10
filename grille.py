n = int(input("Entrez la résolution de la grille (ex: 20) : "))

print(f"\nint etats[RESOLUTION][RESOLUTION] = ")
print("{")

for i in range(n):
    ligne = ",".join(["0"] * n)
    fin = "," if i < n - 1 else ""
    print(f"    {{{ligne}}}{fin}")

print("};")