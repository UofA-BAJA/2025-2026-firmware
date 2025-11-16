using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApplication2.Data;
using WebApplication2.Models;

[ApiController]
// hard code so we don't have to write full name
[Route("temp")]
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
        return await _context.Temperatures.ToListAsync();
    }

    [HttpPost]
    public async Task<IActionResult> Create(Temperature temp)
    {
        temp.Epoch = (int)DateTimeOffset.UtcNow.ToUnixTimeSeconds();
        Console.WriteLine($"DEBUG: BeltId={temp.BeltId}, Value={temp.Value}, Epoch={temp.Epoch}");
        _context.Temperatures.Add(temp);
        await _context.SaveChangesAsync();
        return CreatedAtAction(nameof(GetAll), new { id = temp.Id }, temp);
    }
}
