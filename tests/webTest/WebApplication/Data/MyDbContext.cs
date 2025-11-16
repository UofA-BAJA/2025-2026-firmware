using Microsoft.EntityFrameworkCore;
using WebApplication2.Models;

namespace WebApplication2.Data;
// declare the context for the data base
public class MyDbContext : DbContext
{
    // constructor for the context
    public MyDbContext(DbContextOptions<MyDbContext> options) : base(options) { }
    // holds belts
    public DbSet<Belt> Belts { get; set; }
    // holds temps
    public DbSet<Temperature> Temperatures { get; set; }
}
