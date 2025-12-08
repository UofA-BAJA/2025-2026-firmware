using WebApplication2.Data;
using Microsoft.EntityFrameworkCore;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.IdentityModel.Tokens;
var builder = WebApplication.CreateBuilder(args);

builder.Services.AddOpenApi();
builder.Services.AddControllers();

// authentication and authorization
builder.Services.AddAuthentication(x =>
{
    x.DefaultAuthenticateScheme = JwtBearerDefaults.AuthenticationScheme;
    x.DefaultChallengeScheme = JwtBearerDefaults.AuthenticationScheme;
    x.DefaultScheme = JwtBearerDefaults.AuthenticationScheme;
}).AddJwtBearer(x =>
{
    x.TokenValidationParameters = new TokenValidationParameters
    {
        
    }
});


// set up DB contect for postgresql
builder.Services.AddDbContext<MyDbContext>(options =>
{
    // get the connecting string from the appsettings.json
    var connectionString = builder.Configuration.GetConnectionString("DefaultConnection");
    // set up the connections string
    options.UseNpgsql(connectionString);
});

// set up connections from react
builder.Services.AddCors(options =>
{
    // name the polciy
    options.AddPolicy("AllowReactDev", policy =>
    {
        // configure the port and the allow all api calls from react
        policy
            .WithOrigins("http://localhost:5173")
            .AllowAnyHeader()
            .AllowAnyMethod();
    });
});

var app = builder.Build();

app.UseCors("AllowReactDev");

if (app.Environment.IsDevelopment())
{
    app.MapOpenApi();
}

app.UseHttpsRedirection();
app.MapControllers();
app.UseAuthentication();

app.Run();
