using System.Collections.Generic;

namespace WebApplication2.Models;

public class Belt
{
    public int Id { get; set; }
    public required string Name { get; set; }
    public List<Temperature> Temperatures { get; set; } = new();
}
