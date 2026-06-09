# GeneticAlgorithm

## Uruchomienie

Aby uruchomić program, wystarczy otworzyć folder w Visual Studio i uruchomić w wersji Release.

Program można również uruchomić bez podania żadnych parametrów, wtedy zostaną ustawione parametry domyślne.

```cmd
./GeneticAlgorithm filePath alpha beta gamma delta epsilon maxGeneration treeDepth treeNodeKids maxtime timeScaleEvaluate costScaleEvaluate penaltyEvaluate
```

Opis parametrów:

- filePath - Ścieżka do pliku z grafem zadań
- alpha - Mnożnik populacji
- beta - Prawdopodobieństwo mutacji
- gamma - Prawdopodobieństwo krzyżowania
- delta - Prawdopodobieństwo klonowania
- epsilon - Warunek stopu (ile pokoleń bez poprawy znosimy)
- maxGeneration - Twardy limit pokoleń
- treeDepth - Głębokość drzewa
- treeNodeKids - Maksymalna liczba dzieci w drzewie
- maxtime - Maksymalny dopuszczalny czas
- timeScaleEvaluate - Parametr skalujacy czas wykonania w funkcji fitness
- costScaleEvaluate- Parametr skalujacy koszt w funkcji fitness
- penaltyEvaluate - Parametr skalujący kare za przekroczenie maksymalnego czasu

Przykładowe użycie:

```cmd
./GeneticAlgorithm ../testGraph.txt 2.0 0.3 0.6 0.1 10 20 5 3 20000 10 1 0
```

## Opis

Program stanowi implementację algorytmu genetycznego mającego na celu kosyntezę architektury systemu wbudowanego. Jako operator selekcji, stosowana jest metoda rankingowa. Kryterium stopu stanowy brak lepszych rozwiązań po epsilon prób, jak i twardy limit pokoleń. Ewaluacja pokolenia jest realizowana przy użyciu funkcji kary (mniejszy finess lepszy):
fitness = time _ timeScaleEvaluate + cost _ costScaleEvaluate + penaltyEvaluate max(0, maxTime - time)
