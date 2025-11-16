using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApplication2.Data;
using WebApplication2.Models;

[ApiController]
[Route("[controller]")]
public class BeltsController : ControllerBase
{
    private readonly MyDbContext _context;

    public BeltsController(MyDbContext context)
    {
        _context = context;
    }

    [HttpGet]
    public async Task<IEnumerable<Belt>> GetAll()
    {
        return await _context.Belts.ToListAsync();
    }

    [HttpPost]
    public async Task<IActionResult> Create(Belt belt)
    {
        _context.Belts.Add(belt);
        await _context.SaveChangesAsync();
        // send back belt id
        return CreatedAtAction(nameof(GetAll), new { id = belt.Id }, belt);
    }
}
