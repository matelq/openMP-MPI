# Компилятор и флаги
CXX = mpicxx

# Целевые исполняемые файлы
TARGET = lab3

# Исходные файлы
SOURCE = lab3.cpp

# Количество потоков и размеры векторов
THREADS = 2
SIZES = 21474830

# Файл для сохранения результатов
LOGFILE = "$(TARGET)_results.log"

# Правило по умолчанию (сборка)
build: $(SOURCE)
	$(CXX) -o $(TARGET) $(SOURCE)

# Очистка
clean:
	rm -f $(TARGET) $(LOGFILE)

# Запуск программы с замерами и записью в файл
run: build
	@echo "Результаты запусков:" > $(LOGFILE)
	@for size in $(SIZES); do \
		for threads in $(THREADS); do \
			# echo "Threads = $$threads" >> $(LOGFILE); \
			mpiexec --oversubscribe -n $$threads ./$(TARGET)>> $(LOGFILE); \
		done; \
	done


