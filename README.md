# IDZ3
В файлах client_fork.c и server_fork.c лежат версии с использованием параллельности: когда счетовод получает свою задачу, он её выполняет, а остальные в это время получают новые задачи.
То есть теперь счетовод с номером k+1 может начать работать раньше, чем счетовод k закончит работу
