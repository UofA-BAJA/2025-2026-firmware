using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApplication2.Data;
using WebApplication2.Models;

[ApiController]
[Route("[controller]")]
public class TemperaturesController : ControllerBase
{
    private readonly MyDbContext _context;

    public TemperaturesController(MyDbContext context)
    {
        _context = context;
    }

    [HttpGet]
    public async Task<IEnumerable<Temperature>> GetAll()
    {
        return await _context.Temperatures.Include(t => t.Belt).ToListAsync();
    }

    [HttpPost]
    public async Task<IActionResult> Create(Temperature temp)
    {
        _context.Temperatures.Add(temp);
        await _context.SaveChangesAsync();
        return CreatedAtAction(nameof(GetAll), new { id = temp.Id }, temp);
    }
}
