.PHONY: clean up down reset

clean:
	docker container prune -f
	docker system prune --volumes -f
down: 
	docker compose down

up:
	docker compose up -d
	
build:
	docker compose up --build -d

reset:
	docker compose down
	docker compose down -v
	docker container prune -f
	docker system prune --volumes -f

	docker compose up --build 


