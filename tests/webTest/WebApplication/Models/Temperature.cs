namespace WebApplication2.Models;
using System.ComponentModel.DataAnnotations.Schema;

[Table("temperature")]
public class Temperature
{
    [Column("id")]
    public int Id { get; set; }
    [Column("epoch")]
    public int Epoch { get; set; }
    [Column("value")]
    public int Value { get; set; }
    [Column("belt_id")]
    public int BeltId { get; set; }
}
