using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;
namespace WebApplication2.Models;

[Table("belt")]
public class Belt
{
    [Column("id")]
    public int Id { get; set; }
    [Column("name")]
    public required string Name { get; set; }
}
