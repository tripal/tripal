docker builder prune -f
docker build --progress=plain --no-cache -f Dockerfile-php8.2-windows -t tripalproject/tripaldocker:latest .