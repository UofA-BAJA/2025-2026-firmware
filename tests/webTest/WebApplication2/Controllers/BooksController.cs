using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using WebApplication2.Models;

namespace WebApplication2.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class BooksController : ControllerBase
    {
        static private List<Book> books = new List<Book>
        {
            new Book { Id = 1, Title = "The Silent Horizon", Author = "Amelia Carter", YearPublished = 2005 },
            new Book { Id = 2, Title = "Whispers of the Past", Author = "Liam Johnson", YearPublished = 2012 },
            new Book { Id = 3, Title = "Echoes in the Valley", Author = "Sophia Martinez", YearPublished = 1999 },
            new Book { Id = 4, Title = "Shadows of Tomorrow", Author = "Daniel Thompson", YearPublished = 2018 },
            new Book { Id = 5, Title = "The Forgotten Voyage", Author = "Isabella Chen", YearPublished = 2001 },
            new Book { Id = 6, Title = "Crimson Skies", Author = "Oliver White", YearPublished = 2015 },
            new Book { Id = 7, Title = "Dreams of Ashes", Author = "Maya Patel", YearPublished = 2008 },
            new Book { Id = 8, Title = "Beneath the Iron Sea", Author = "Ethan Brown", YearPublished = 2020 },
            new Book { Id = 9, Title = "Fragments of Eternity", Author = "Chloe Wilson", YearPublished = 1995 },
            new Book { Id = 10, Title = "The Last Ember", Author = "Noah Garcia", YearPublished = 2023 }
        };
        [HttpGet]
        public ActionResult<List<Book>> GetBooks()
        {
            return Ok(books);
        }

        [HttpGet("{id}")]
        public ActionResult<Book> GetBook(int id)
        {
            var book = books.FirstOrDefault(x => x.Id == id);
            if (book == null)
            {
                return NotFound();
            }
            return Ok(book);
        }
        [HttpPost]
        public ActionResult<Book> AddBook(Book newBook)
        {
            if (newBook == null)
            {
                return BadRequest();
            }
            books.Add(newBook);
            return CreatedAtAction(nameof(GetBook), new { id = newBook.Id }, newBook);
        }

    }

}