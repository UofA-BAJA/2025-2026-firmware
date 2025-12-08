.PHONY: clean

.PHONY: up

.PHONY: down

clean:
	docker container prune
	docker system prune --volumes	
down: 
	docker compose down

up:
	docker compose up -d

build:
	docker compose up --build -d
